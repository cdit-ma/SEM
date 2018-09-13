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
 
#include "protorequester.hpp"
#include <iostream>
#include "../zmqutils.hpp"

zmq::ProtoRequester::ProtoRequester(){
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
}

zmq::ProtoRequester::~ProtoRequester(){
    Terminate();
}

void zmq::ProtoRequester::Connect(const std::string& address){
    connect_address_ = address;
}

void zmq::ProtoRequester::Start(){
    if(!zmq_requester_.valid()){
        zmq_requester_ = std::async(std::launch::async, &zmq::ProtoRequester::ZmqRequester, this);
    }
}

void zmq::ProtoRequester::Terminate(){

}

std::future<std::unique_ptr<google::protobuf::MessageLite> > 
zmq::ProtoRequester::EnqueueRequest(
    const std::string& function_name,
    const google::protobuf::MessageLite& request_proto,
    const google::protobuf::MessageLite& reply_default_instance)
{

    RequestStruct request{String2Zmq(function_name),
                            String2Zmq(request_proto.GetTypeName()),
                            Proto2Zmq(request_proto)
                        };

    auto future = request.promise.get_future();
    //Enqneue

    std::lock_guard<std::mutex> lock(queue_mutex_);
    request_queue_.emplace(std::move(request));
    queue_lock_condition_.notify_all();
    return future;
}

void zmq::ProtoRequester::ZmqRequester(){
    zmq::socket_t socket(*context_, ZMQ_REQ);

    //Connect to all nodes on our network
    try{
        socket.connect(connect_address_.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "zmq::ProtoRequester: Failed to connect to address: '" << connect_address_ << "' " << ex.what() << std::endl;
    }
    try{
        bool running = true;
        while(running){
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            //Wait until we have messages or are told to terminate
            queue_lock_condition_.wait(lock, [=]{return terminate_ || request_queue_.size();});

            if(request_queue_.size()){
                auto request = std::move(request_queue_.front());
                request_queue_.pop();
                //Release the mutex
                lock.unlock();

                //Send the request
                socket.send(request.function_name, ZMQ_SNDMORE);
                socket.send(request.request_typename, ZMQ_SNDMORE);
                socket.send(request.request_data);

                zmq::message_t zmq_result;
                socket.recv(&zmq_result);

                uint32_t result_code = *((uint32_t*)zmq_result.data());
                //Test the result
                if(result_code == 0){
                    zmq::message_t zmq_reply_typename;
                    zmq::message_t zmq_reply_data;

                    socket.recv(&zmq_reply_typename);
                    socket.recv(&zmq_reply_data);

                    const auto& str_reply_typename = Zmq2String(zmq_reply_typename);

                    bool got_request_constructor = proto_constructor_lookup_.count(str_reply_typename) > 0;

                    if(got_request_constructor){
                        //Construct the Reply Protobuf message
                        auto reply_type = proto_constructor_lookup_[str_reply_typename](zmq_reply_data);
                        request.promise.set_value(std::move(reply_type));
                    }else{
                        const auto& exception = std::runtime_error("zmq::ProtoRequester: Doesn't have Proto '" + str_reply_typename + "' Registered!");
                        request.promise.set_exception(std::make_exception_ptr(exception));
                    }
                }else{
                    //Error
                    zmq::message_t zmq_error_data;
                    socket.recv(&zmq_error_data);

                    const auto& str_exception_str = Zmq2String(zmq_error_data);

                    const auto& exception = std::runtime_error("zmq::ProtoRequester: Function Exception '" + str_exception_str + "'");
                    request.promise.set_exception(std::make_exception_ptr(exception));
                }
            }
        }
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::cerr << "zmq::ProtoRequester: ZMQ Exception: " << ex.what() << std::endl;
        }
    }
}

void zmq::ProtoRequester::RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance){
    const auto& type_name = default_instance.GetTypeName();

    if(!proto_constructor_lookup_.count(type_name)){
        //Insert a factory function which will construct a protobuf message from the zmq_message
        proto_constructor_lookup_[type_name] = [&default_instance](const zmq::message_t& zmq_message){
            //Construct a Protobuf message
            auto pb_obj = std::unique_ptr<google::protobuf::MessageLite>(default_instance.New());

            if(!pb_obj->ParseFromArray(zmq_message.data(), zmq_message.size())){
                //If message failed to parse, clean up
                pb_obj.reset();
            }
            return pb_obj;
        };
    }
}