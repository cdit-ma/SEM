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
#include <memory>
#include <future>

#include <zmq.hpp>
#include <vector>
#include <google/protobuf/message_lite.h>
#include "../zmqutils.hpp"
#include "../protoregister/protoregister.h"


namespace zmq{
    class TimeoutException : public std::runtime_error{
        public:
            TimeoutException(const std::string& what_arg) : std::runtime_error(what_arg){};
    };

    class RMIException : public std::runtime_error{
        public:
            RMIException(const std::string& what_arg) : std::runtime_error(what_arg){};
    };

    class ProtoRequester{
        public:
            ProtoRequester(const std::string& address);
            ~ProtoRequester();
            
            template<class RequestType, class ReplyType>
            std::future<std::unique_ptr<ReplyType> > SendRequest(const std::string& function_name, const RequestType& request, const int timeout_ms);
        private:
            std::future<std::unique_ptr<google::protobuf::MessageLite> > SendRequest(const std::string& fn_signature, const google::protobuf::MessageLite& request_proto, const int timeout_ms);
            void ProcessRequest(const std::string fn_signature, const std::string type_name, const std::string data, std::promise<std::unique_ptr<google::protobuf::MessageLite>> promise,  const int timeout_ms);
            zmq::socket_t GetRequestSocket();
        private:
            const std::string connect_address_;
            ProtoRegister proto_register_;
            
            std::mutex zmq_mutex_;
            std::unique_ptr<zmq::context_t> context_;

            std::mutex future_mutex_;
            std::vector<std::future<void> > futures_;
    };
};

template<class RequestType, class ReplyType>
std::future<std::unique_ptr<ReplyType> > zmq::ProtoRequester::SendRequest(const std::string& function_name, const RequestType& request, const int timeout_ms){
    static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
    static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");

    //Register the proto constructors
    proto_register_.RegisterProtoConstructor(RequestType::default_instance());
    proto_register_.RegisterProtoConstructor(ReplyType::default_instance());
    
    //Get the function signature
    auto fn_signature = zmq::GetFunctionSignature(function_name, RequestType::default_instance(), ReplyType::default_instance());
    auto future = SendRequest(fn_signature, request, timeout_ms);

    //Do the up casting
    return std::async(std::launch::deferred, [](std::future<std::unique_ptr<google::protobuf::MessageLite> > future) {
        if(future.valid()){
            auto reply = future.get();
            if(reply){
                //Get the raw pointer from the unique_ptr
                auto reply_ptr = reply.release();
                //Upcast and return 
                return std::unique_ptr<ReplyType>(dynamic_cast<ReplyType*>(reply_ptr));
            }
        }
        throw std::runtime_error("Invalid Future");
    }, std::move(future));
};

#endif //RE_COMMON_ZMQ_PROTOREQUESTER_H