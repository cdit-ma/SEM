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

zmq::ProtoRequester::ProtoRequester(const std::string& connect_address):
    connect_address_(connect_address),
    context_{new zmq::context_t(1)}
{
    assert(context_);
    assert(connect_address_.length() > 0);
    requester_future_ = std::async(std::launch::async, &zmq::ProtoRequester::ProcessRequests, this);
}

zmq::ProtoRequester::~ProtoRequester(){
    {
        //Shutting down the contexts forces all zmq_sockets to stop blocking on recv and throw and exceptions.
        //Which should terminate all async requests
        std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
        context_.reset();
        request_cv_.notify_all();
    }

    //Wait for all futures to die before allowing the requester to die
    std::lock_guard<std::mutex> future_lock(future_mutex_);
    try{
        requester_future_.get();
    }catch(const std::exception& ex){

    }
}

std::future<std::unique_ptr<google::protobuf::MessageLite> > 
zmq::ProtoRequester::SendRequest(const std::string& fn_signature, const google::protobuf::MessageLite& request_proto, const int timeout_ms){
    auto request = std::unique_ptr<RequestStruct>(new RequestStruct({fn_signature, request_proto.GetTypeName(), request_proto.SerializeAsString(), std::chrono::milliseconds(timeout_ms)}));

    auto result_future = request->promise.get_future();
    
    //Insert the request_future into a list of futures this object is responsible for cleaning up
    std::lock_guard<std::mutex> lock(request_mutex_);
    request_queue_.emplace_back(std::move(request));
    request_cv_.notify_all();

    //Return the result future
    return std::move(result_future);
}

zmq::socket_t zmq::ProtoRequester::GetRequestSocket(){
    std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
    if(context_){
        try{
            zmq::socket_t socket(*context_, ZMQ_REQ);
            //Setting the linger duration will mean that terminating the context won't wait on child sockets to be terminated
            socket.setsockopt(ZMQ_LINGER, 0);
            socket.connect(connect_address_.c_str());
            return std::move(socket);

        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                throw std::runtime_error("Failed to connect to address: '" + connect_address_ + "': " + ex.what());
            }
        }
    }
    throw std::runtime_error("Got Invalid Context");
}

void zmq::ProtoRequester::ProcessRequests(){
    auto socket = GetRequestSocket();

    
    while(true){
        std::unique_ptr<RequestStruct> request;

        {
            std::unique_lock<std::mutex> lock(request_mutex_);
            request_cv_.wait(lock, [this]{
                std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
                //Wake up if the termination flag has been set
                return !context_ || request_queue_.size();
            });
            if(!context_){
                request_queue_.clear();
                return;
            }

            if(request_queue_.size()){
                request = std::move(request_queue_.front());
                request_queue_.pop_front();
            }
        }
        

        if(!request){
            continue;
        }

        try{
            //Send the request
            socket.send(String2Zmq(request->fn_signature), ZMQ_SNDMORE);
            socket.send(String2Zmq(request->type_name), ZMQ_SNDMORE);
            socket.send(String2Zmq(request->data));
            
            int events = zmq::poll({{socket, 0, ZMQ_POLLIN, 0}}, request->timeout.count());

            if(events > 0){
                zmq::message_t zmq_result;
                socket.recv(&zmq_result);

                //We are expecting an int32 as the result flag, so convert from void* to int*, then dereference
                uint32_t result_code = *((uint32_t*)zmq_result.data());

                if(result_code == 0){
                    //Success
                    zmq::message_t zmq_reply_typename;
                    zmq::message_t zmq_reply_data;

                    socket.recv(&zmq_reply_typename);
                    socket.recv(&zmq_reply_data);

                    const auto& str_reply_typename = Zmq2String(zmq_reply_typename);

                    auto proto_reply = proto_register_.ConstructProto(str_reply_typename, zmq_reply_data);
                    request->promise.set_value(std::move(proto_reply));
                }else{
                    //Received Error from the Replier
                    zmq::message_t zmq_error_data;
                    socket.recv(&zmq_error_data);

                    //Receiving a string'd exception
                    const auto& str_exception_str = Zmq2String(zmq_error_data);
                    request->promise.set_exception(std::make_exception_ptr(RMIException(str_exception_str)));
                }
            }else{
                //If we didn't receive any events, throw a timeout exception
                request->promise.set_exception(std::make_exception_ptr(TimeoutException("Request timed out")));
            }
        }catch(...){
            //Catch all exceptions and pass them up the to the promise
            request->promise.set_exception(std::current_exception());
            socket = GetRequestSocket();
        }
    }
}
