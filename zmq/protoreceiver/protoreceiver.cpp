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

zmq::ProtoReceiver::ProtoReceiver():
inproc_address_("inproc://util_socket")
{
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    
    const auto&& inproc_socket = zmq::socket_t(*context_, ZMQ_SUB);
    try{
        inproc_socket.bind(inproc_address_.c_str());
        inproc_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }catch(const zmq::error_t& ex){
        throw std::runtime_error("zmq::ProtoReceiver: Failed to bind inproc address for util socket");
    }

    zmq_receiver_ = std::async(std::launch::async, &zmq::ProtoReceiver::ZmqReceiver, this, inproc_socket);
    proto_converter_ = std::async(std::launch::async, &zmq::ProtoReceiver::ProtoConverter, this);
}

zmq::ProtoReceiver::~ProtoReceiver(){
    Terminate();
}

zmq::socket_t zmq::ProtoReceiver::GetSubSocket(){
    std::lock_guard<std::mutex> lock(zmq_mutex_);
    return zmq::socket_t(*context_, ZMQ_SUB);
}

zmq::socket_t zmq::ProtoReceiver::GetInprocSocket(){
    try{
        const auto&& inproc_socket = GetSubSocket();
        inproc_socket.connect(inproc_address_.c_str());
        return inproc_socket;
    }catch(const zmq::error_t& ex){
        throw std::runtime_error("zmq::ProtoReceiver: Failed to bind inproc address for util socket");
    }
}

void zmq::ProtoReceiver::Start(){

}

void zmq::ProtoReceiver::SetBatchMode(size_t batch_size){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    batch_size_ = batch_size;
}

void zmq::ProtoReceiver::Filter(const std::string& filter){
    auto socket = GetInprocSocket();
    socket.send(String2Zmq("filter"), ZMQ_SNDMORE);
    socket.send(String2Zmq(filter));
}

void zmq::ProtoReceiver::Unfilter(const std::string& filter){
    auto socket = GetInprocSocket();
    socket.send(String2Zmq("unfilter"), ZMQ_SNDMORE);
    socket.send(String2Zmq(filter));
}

void zmq::ProtoReceiver::Connect(const std::string& address){
    auto socket = GetInprocSocket();
    socket.send(String2Zmq("connect"), ZMQ_SNDMORE);
    socket.send(String2Zmq(address));
}

void zmq::ProtoReceiver::Disconnect(const std::string& address){
    auto socket = GetInprocSocket();
    socket.send(String2Zmq("disconnect"), ZMQ_SNDMORE);
    socket.send(String2Zmq(address));
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


void zmq::ProtoReceiver::ZmqReceiver(zmq::socket_t inproc_socket){
    auto message_socket = GetSubSocket();

    zmq::pollitem_t poll_items[] = {
        {inproc_socket, 0, ZMQ_POLLIN, 0},
        {message_socket, 0, ZMQ_POLLIN, 0}
    };

    while(true){
}

void zmq::ProtoReceiver::RegisterNewProto(const google::protobuf::MessageLite& message_default_instance, std::function<void(const google::protobuf::MessageLite&)> callback_function){
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