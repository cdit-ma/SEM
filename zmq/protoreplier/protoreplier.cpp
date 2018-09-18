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
 
#include "protoreplier.hpp"
#include <iostream>

zmq::ProtoReplier::ProtoReplier(){
}

zmq::ProtoReplier::~ProtoReplier(){
    Terminate();
}

void zmq::ProtoReplier::Bind(const std::string& address){
    std::lock_guard<std::mutex> lock(address_mutex_);
    bind_addresses_.emplace(address);
}

std::future<void> zmq::ProtoReplier::Start(const std::vector<std::chrono::milliseconds>& retry_timeouts){
    std::future<void> blocking_future;

    std::lock_guard<std::mutex> future_lock(future_mutex_);
    if(future_.valid()){
        throw std::logic_error("Replier already Started");
    }

    auto socket = GetReplySocket();
    std::promise<void> blocking_promise;
    blocking_future = blocking_promise.get_future();
    future_ = std::async(std::launch::async, &zmq::ProtoReplier::ZmqReplier, this, std::move(socket), retry_timeouts, std::move(blocking_promise));

    return std::move(blocking_future);
}

void zmq::ProtoReplier::Terminate(){
    std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
    std::lock_guard<std::mutex> future_lock(future_mutex_);

    //Shutting down the contexts forces all zmq_sockets to stop blocking on recv and throw and exceptions.
    //Which should terminate all async requests
    if(context_){
        context_.reset();
    }

    //Wait until the task ZmqReplier is finished
    if(future_.valid()){
        future_.get();
    }
}

zmq::socket_t zmq::ProtoReplier::GetReplySocket(){
    std::lock_guard<std::mutex> address_lock(address_mutex_);
    std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);

    if(!context_){
        context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    }

    if(context_){
        zmq::socket_t socket(*context_, ZMQ_REP);
        //Setting the linger duration will mean that terminating the context won't wait on child sockets to be terminated
        socket.setsockopt(ZMQ_LINGER, 0);

        if(bind_addresses_.empty()){
            throw std::logic_error("No bound addresses");
        }

        //Connect to all nodes on our network
        for (const auto& address : bind_addresses_){
            try{
                socket.bind(address.c_str());
            }catch(const zmq::error_t& ex){
                throw std::runtime_error("Failed to connect to Address: " + address);
            }
        }
        return std::move(socket);
    }
    throw std::runtime_error("Got Invalid Context");
}

void zmq::ProtoReplier::ZmqReplier(zmq::socket_t socket, const std::vector<std::chrono::milliseconds> retry_timeouts, std::promise<void> blocked_promise){
    try{
        int retry_count = 0;
        
        while(true){
            //zmq::poll with -1 timeout blocks until new message is returned
            std::chrono::milliseconds poll_ms(-1);
            
            if(retry_timeouts.size()){
                if(retry_count < retry_timeouts.size()){
                    //Get the next timeout time
                    poll_ms = retry_timeouts.at(retry_count);
                }else{
                    throw TimeoutException("Replier Timed out");
                }
            }

            try{
                //Try and wait, with no retry_timeouts will poll indefinetely
                auto events_rx = zmq::poll({{socket, 0, ZMQ_POLLIN, 0}}, poll_ms.count());
                
                if(events_rx > 0){
                    //Reset our retry count
                    retry_count = 0;

                    zmq::message_t zmq_function_name;
                    zmq::message_t zmq_request_typename;
                    zmq::message_t zmq_request_data;
                    
                    //Recieve Our Message
                    socket.recv(&zmq_function_name);
                    socket.recv(&zmq_request_typename);
                    socket.recv(&zmq_request_data);

                    //Get the type of the protobuf message
                    const auto& str_fn_signature = Zmq2String(zmq_function_name);
                    const auto& str_request_typename = Zmq2String(zmq_request_typename);

                    std::string error_str;
                    std::unique_ptr<google::protobuf::MessageLite> reply_proto;

                    try{
                        //Try and construct the proto type
                        auto request_proto = proto_register_.ConstructProto(str_request_typename, zmq_request_data);

                        std::unique_lock<std::mutex> callback_lock(callback_mutex_);
                        if(callback_lookup_.count(str_fn_signature)){
                            //Run the Callback function
                            reply_proto = callback_lookup_[str_fn_signature](*request_proto);
                        }else{
                            throw std::runtime_error("No registered function: '" + str_fn_signature + "'");
                        }
                    }catch(const std::exception& ex){
                        error_str = ex.what();
                        std::cerr << "zmq::ProtoReplier::ZmqReplier(): " << ex.what() << std::endl;
                    }

                    //Construct a success flag message
                    uint32_t success = (reply_proto == nullptr) ? 1 : 0;
                    //Construct a success message
                    zmq::message_t zmq_result(&success, sizeof(success));

                    //Success
                    if(reply_proto){
                        auto zmq_reply_typename = String2Zmq(reply_proto->GetTypeName());
                        auto zmq_reply_data = Proto2Zmq(*reply_proto);

                        socket.send(zmq_result, ZMQ_SNDMORE);
                        socket.send(zmq_reply_typename, ZMQ_SNDMORE);
                        socket.send(zmq_reply_data);
                    }else{
                        auto zmq_reply_error = String2Zmq(error_str);
                        socket.send(zmq_result, ZMQ_SNDMORE);
                        socket.send(zmq_reply_error);
                    }
                }else{
                    if(retry_timeouts.size()){
                        retry_count ++;
                    }
                }
            }catch(const zmq::error_t& ex){
                if(ex.num() != ETERM){
                    std::cerr << "zmq::ProtoReplier: ZMQ Exception: " << ex.what() << std::endl;
                }
                break;
            }
        }
    }catch(...){
        //Set the exception caught on the blocking future
        blocked_promise.set_exception(std::current_exception());
    }
}

void zmq::ProtoReplier::RegisterNewProto(const std::string& fn_signature, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> callback_function){
    std::unique_lock<std::mutex> callback_lock(callback_mutex_);
    if(!callback_lookup_.count(fn_signature)){
        callback_lookup_[fn_signature] = callback_function;
    }else{
        throw std::invalid_argument("Request Type '" + fn_signature + "' Already Registered");
    }
}