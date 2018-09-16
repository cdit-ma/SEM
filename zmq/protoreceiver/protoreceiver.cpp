/* re_common
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "re_common"
 *
 * "re_common" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "re_common" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "protoreceiver.h"
#include <iostream>
#include "../zmqutils.hpp"

zmq::ProtoReceiver::ProtoReceiver(){

}

zmq::ProtoReceiver::~ProtoReceiver(){
    Terminate();
}

void zmq::ProtoReceiver::Start(){
    std::lock_guard<std::mutex> lock(future_mutex_);

    if(!zmq_receiver_.valid() && !proto_converter_.valid()){
        {
            //Construct a context
            std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
            context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
        }

        //Start our two async tasks
        terminate_proto_converter_ = false;
        zmq_receiver_ = std::async(std::launch::async, &zmq::ProtoReceiver::ZmqReceiver, this);
        proto_converter_ = std::async(std::launch::async, &zmq::ProtoReceiver::ProtoConverter, this);
    }else{
        throw std::runtime_error("zmq::ProtoReceiver already active.");
    }
}

void zmq::ProtoReceiver::SetBatchMode(size_t batch_size){
    std::unique_lock<std::mutex> lock(queue_mutex_);
    batch_size_ = batch_size;
}

void zmq::ProtoReceiver::Filter(const std::string& filter){
    std::unique_lock<std::mutex> lock(zmq_mutex_);
    filters_.insert(filter);
}

void zmq::ProtoReceiver::Connect(const std::string& address){
    std::unique_lock<std::mutex> lock(zmq_mutex_);
    addresses_.insert(address);
}

void zmq::ProtoReceiver::Terminate(){
    std::lock_guard<std::mutex> lock(future_mutex_);

    if(zmq_receiver_.valid()){
        {
            //Destroy the context
            std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
            context_.reset();
        }

        //Wait for the zmq receiver to finish
        zmq_receiver_.wait();
    }

    if(proto_converter_.valid()){
        {
            //Set the terminate flag
            std::unique_lock<std::mutex> lock(queue_mutex_);
            terminate_proto_converter_ = true;
            queue_lock_condition_.notify_all();
        }

        //Wait for the proto converter to finish
        proto_converter_.wait();
    }
}



void zmq::ProtoReceiver::ZmqReceiver(){
    std::unique_ptr<zmq::socket_t> socket;
    {
        std::lock_guard<std::mutex> lock(zmq_mutex_);
        if(context_){
            socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_SUB));
        }

        //Connect to all nodes on our network
        for (const auto& address : addresses_){
            try{
                socket->connect(address.c_str());
            }catch(const zmq::error_t& ex){
                std::cerr << "zmq::ProtoReceiver: Failed to connect to address: '" << address << "' " << ex.what() << std::endl;
            }
        }

        //Apply all filters
        for (const auto& filter : filters_){
            try{
                socket->setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
            }catch(const zmq::error_t& ex){
                std::cerr << "zmq::ProtoReceiver: Failed to set filter: '" << filter << "' " << ex.what() << std::endl;
            }
        }
    }
    
    while(true){
		try{
            zmq::message_t topic;
            std::pair<zmq::message_t, zmq::message_t> message_pair;

            //Wait for Topic, Type and Data
            socket->recv(&topic);
            socket->recv(&(message_pair.first));
            socket->recv(&(message_pair.second));
            
            bool notify = false;
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                rx_message_queue_.emplace(std::move(message_pair));
                notify = rx_message_queue_.size() > batch_size_;
            }

            if(notify){
                //Notify the ProtoConverter
                queue_lock_condition_.notify_all();
            }
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::ProtoReceiver: ZMQ Exception: " << ex.what() << std::endl;
            }
            break;
        }
    }
}

void zmq::ProtoReceiver::RegisterNewProto(const google::protobuf::MessageLite& message_default_instance, std::function<void(const google::protobuf::MessageLite&)> callback_function){
    proto_register_.RegisterProtoConstructor(message_default_instance);
    
    std::unique_lock<std::mutex> lock(callback_mutex_);
    const auto& proto_type_name = message_default_instance.GetTypeName();
    //Insert a callback function for this proto type
    callback_lookup_.insert({proto_type_name, callback_function});
}

int zmq::ProtoReceiver::GetRxCount(){
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return rx_count_;
}

void zmq::ProtoReceiver::ProcessMessage(const zmq::message_t& proto_type, const zmq::message_t& proto_data){
    //Get the type of the protobuf message
    const auto& proto_type_str = Zmq2String(proto_type);
    auto protobuf_message = proto_register_.ConstructProto(proto_type_str, proto_data);

    std::unique_lock<std::mutex> lock(callback_mutex_);
    if(protobuf_message){
        //Increment our count function
        rx_count_++;
        
        //Find all callbacks with the type proto_type_str
        auto callback_range = callback_lookup_.equal_range(proto_type_str);

        //Callback into all callbacks
        for (auto i = callback_range.first; i != callback_range.second; ++i){
            i->second(*protobuf_message);
        }
    }
}

void zmq::ProtoReceiver::ProtoConverter(){
    bool running = true;
    while(running){
        std::queue< std::pair<zmq::message_t, zmq::message_t> > queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait until we have messages or are told to terminate
            queue_lock_condition_.wait(lock, [=]{
                return terminate_proto_converter_ || rx_message_queue_.size();
            });

            if(terminate_proto_converter_){
                //Break out if we need to terminat
                running = false;
            }
            
            //Swap queues
            queue.swap(rx_message_queue_);
        }

        while(!queue.empty()){
            const auto& type = queue.front().first;
            const auto& msg = queue.front().second;

            try{
                ProcessMessage(type, msg);
            }catch(const std::exception& ex){
                std::cerr << "Cannot Process Message: " << ex.what() << std::endl;
            }
            queue.pop();
        }
    }
}