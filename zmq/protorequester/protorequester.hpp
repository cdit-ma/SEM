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
 
#ifndef RE_COMMON_ZMQ_PROTOREQUESTER_H
#define RE_COMMON_ZMQ_PROTOREQUESTER_H

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include <list>
#include <memory>
#include <future>

#include <zmq.hpp>

#include <google/protobuf/message_lite.h>
#include "../zmqutils.hpp"
#include "../protoregister/protoregister.hpp"


namespace zmq{
    class ProtoRequester{
        public:
            ProtoRequester(const std::string& address);
            ~ProtoRequester();

            template<class RequestType, class ReplyType>
            std::future<std::unique_ptr<ReplyType> > SendRequest(const std::string& function_name, const RequestType& request, const int timeout_ms);
        private:
            std::future<std::unique_ptr<google::protobuf::MessageLite> > SendRequest(const std::string& fn_signature, const google::protobuf::MessageLite& request_proto, const int timeout_ms);
            zmq::socket_t GetRequestSocket();
        private:
            const std::string connect_address_;
            ProtoRegister proto_register_;

            struct RequestStruct{
                const std::string fn_signature;
                const std::string type_name;
                const std::string data;
                const std::chrono::milliseconds timeout; 
                std::promise<std::unique_ptr<google::protobuf::MessageLite>> promise;
            };

            void ProcessRequests();

            std::mutex request_mutex_;
            std::condition_variable request_cv_;

            std::mutex future_mutex_;
            std::future<void> requester_future_;

            std::mutex zmq_mutex_;
            std::unique_ptr<zmq::context_t> context_;


            std::list< std::unique_ptr<RequestStruct> > request_queue_;
    };
};

template<class RequestType, class ReplyType>
std::future<std::unique_ptr<ReplyType> > zmq::ProtoRequester::SendRequest(const std::string& function_name, const RequestType& request, const int timeout_ms){
    static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
    static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");

    //Register the callbacks
    proto_register_.RegisterProto<RequestType>();
    proto_register_.RegisterProto<ReplyType>();
    
    //Get the function signature
    const auto& fn_signature = zmq::GetFunctionSignature<RequestType, ReplyType>(function_name);

    auto future = SendRequest(fn_signature, request, timeout_ms);

    //Do the up casting
    return std::async(std::launch::deferred, [](std::future<std::unique_ptr<google::protobuf::MessageLite> > future) {
        if(future.valid()){
            auto reply = future.get();
            if(reply){
                auto reply_proto_ptr = dynamic_cast<ReplyType*>(reply.get());
                if(reply_proto_ptr){
                    //release the unique pointer
                    reply.release();
                    //Upcast and return 
                    return std::unique_ptr<ReplyType>(reply_proto_ptr);
                }else{
                    throw std::runtime_error("Got Invalid ProtoType: " + reply->GetTypeName());
                }
            }
        }
        throw std::runtime_error("Invalid Future");
    }, std::move(future));
};

#endif //RE_COMMON_ZMQ_PROTOREQUESTER_H