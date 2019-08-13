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
inproc_address_("inproc://ProtoReceiver")
{
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    
    auto inproc_socket =  std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_REP));
    try{
        inproc_socket->setsockopt(ZMQ_LINGER, 0);
        inproc_socket->setsockopt(ZMQ_RCVHWM, 0);
        inproc_socket->bind(inproc_address_.c_str());
    }catch(const zmq::error_t& ex){
        throw std::runtime_error("zmq::ProtoReceiver: Failed to bind inproc address for util socket");
    }

    zmq_receiver_ = std::async(std::launch::async, &zmq::ProtoReceiver::ZmqReceiver, this, std::move(inproc_socket));
    proto_converter_ = std::async(std::launch::async, &zmq::ProtoReceiver::ProtoConverter, this);
}

zmq::ProtoReceiver::~ProtoReceiver(){
    if(zmq_receiver_.valid()){
        SendInprocMessage("terminate", "");
        try{
            zmq_receiver_.get();
            std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
            context_.reset();
        }catch(const std::exception& ex){
            std::cerr << "zmq::ProtoReceiver:ZmqAsync: " << ex.what() << std::endl;
        }
    }

    if(proto_converter_.valid()){
        {
            //Set the terminate flag
            std::unique_lock<std::mutex> lock(queue_mutex_);
            terminate_proto_converter_ = true;
        }
        queue_lock_condition_.notify_all();

        try{
            proto_converter_.get();
        }catch(const std::exception& ex){
            std::cerr << "zmq::ProtoReceiver:ProtoAsync: " << ex.what() << std::endl;
        }
    }
}

std::unique_ptr<zmq::socket_t> zmq::ProtoReceiver::GetSubSocket(){
    std::lock_guard<std::mutex> lock(zmq_mutex_);
    auto socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_SUB));
    socket->setsockopt(ZMQ_LINGER, 1000);
    socket->setsockopt(ZMQ_RCVHWM, 0);
    return socket;
}


std::unique_ptr<zmq::socket_t> zmq::ProtoReceiver::GetInprocSocket(){
    try{
        std::lock_guard<std::mutex> lock(zmq_mutex_);
        auto socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_REQ));
        socket->setsockopt(ZMQ_LINGER, -1);
        socket->connect(inproc_address_.c_str());
        return socket;
    }catch(const zmq::error_t& ex){
        throw std::runtime_error("zmq::ProtoReceiver: Failed to bind inproc address for util socket");
    }
}

void zmq::ProtoReceiver::SetBatchMode(size_t batch_size){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    batch_size_ = batch_size;
}

void zmq::ProtoReceiver::SendInprocMessage(const std::string& function, const std::string& args){
    auto socket = GetInprocSocket();
    socket->send(String2Zmq(function), ZMQ_SNDMORE);
    socket->send(String2Zmq(args));
    auto events = zmq::poll({{*socket, 0, ZMQ_POLLIN, 0}}, 100);
    if(!events){
        throw std::runtime_error("zmq::ProtoReceiver(): Sending inproc message has timed out. Something is very wrong");
    }
}

void zmq::ProtoReceiver::Filter(const std::string& filter){
    SendInprocMessage("filter", filter);
}

void zmq::ProtoReceiver::Unfilter(const std::string& filter){
    SendInprocMessage("unfilter", filter);
}

void zmq::ProtoReceiver::Connect(const std::string& address){
    SendInprocMessage("connect", address);
}

void zmq::ProtoReceiver::Disconnect(const std::string& address){
    SendInprocMessage("disconnect", address);
}

void zmq::ProtoReceiver::ZmqReceiver(std::unique_ptr<zmq::socket_t> inproc_socket){
    auto message_socket = GetSubSocket();
    
    zmq::pollitem_t poll_items[] = {
        {*inproc_socket, 0, ZMQ_POLLIN, 0},
        {*message_socket, 0, ZMQ_POLLIN, 0}
    };

    bool running = true;
    while(running){
        try{
            int timeout = running ? -1 : 1000;
            //Wait for events
            zmq::poll(&poll_items[0], 2, timeout);
            
            bool recv_any = false;
            if(poll_items[0].revents & ZMQ_POLLIN){
                recv_any = true;
                zmq::message_t function;
                zmq::message_t args;
                inproc_socket->recv(&function);
                inproc_socket->recv(&args);

                const auto& function_str = Zmq2String(function);
                const auto& args_str = Zmq2String(args);
                
                try{
                    if(function_str == "connect"){
                        message_socket->connect(args_str.c_str());
                    }else if(function_str == "disconnect"){
                        message_socket->disconnect(args_str.c_str());
                    }else if(function_str == "filter"){
                        message_socket->setsockopt(ZMQ_SUBSCRIBE, args_str.c_str(), args_str.size());
                    }else if(function_str == "unfilter"){
                        message_socket->setsockopt(ZMQ_UNSUBSCRIBE, args_str.c_str(), args_str.size());
                    }else if(function_str == "terminate"){
                        running = false;    
                    }else{
                        throw std::runtime_error("Got Invalid Function: '" + function_str + "'");
                    }
                }catch(const zmq::error_t& ex){
                    if(ex.num() != ETERM){
                        std::cerr << "zmq::ProtoReplier: Handling Function '" + function_str + "' with Args: '" + args_str + "' threw error: '" + ex.what() << std::endl;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                inproc_socket->send(zmq::message_t());
                
                //if(running){
                    //continue;
                //}
            }

            if (poll_items[1].revents & ZMQ_POLLIN) {
                recv_any = true;
                zmq::message_t topic;
                std::pair<zmq::message_t, zmq::message_t> message_pair;

                //Wait for Topic, Type and Data
                message_socket->recv(&topic);
                message_socket->recv(&(message_pair.first));
                message_socket->recv(&(message_pair.second));

                bool notify = false;
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    rx_message_queue_.emplace(std::move(message_pair));
                    notify = !running || rx_message_queue_.size() > batch_size_;
                }

                if(notify){
                    //Notify the ProtoConverter
                    queue_lock_condition_.notify_all();
                }
            }
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                throw;
            }else{
                break;
            }
        }
    }
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