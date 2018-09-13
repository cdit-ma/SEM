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
#include <set>
#include <future>
#include <queue>
#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
#include "../zmqutils.hpp"

namespace zmq{
    class ProtoRequester{
        public:
            ProtoRequester();
            ~ProtoRequester();

            void Connect(const std::string& address);
            
            void Start();
            void Terminate();


            template<class RequestType, class ReplyType>
            std::future<std::unique_ptr<ReplyType> > SendRequest(const std::string& function_name, const RequestType& request);

        private:
            std::future<std::unique_ptr<google::protobuf::MessageLite> > EnqueueRequest(const std::string& function_name, const google::protobuf::MessageLite& request, const google::protobuf::MessageLite& reply_default_instance);

            struct RequestStruct{
                zmq::message_t function_name;
                zmq::message_t request_typename;
                zmq::message_t request_data;
                std::promise<std::unique_ptr<google::protobuf::MessageLite> > promise;
            };

            void RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance);

            void ZmqRequester();


            std::queue<RequestStruct> request_queue_;

            std::unique_ptr<zmq::context_t> context_;

            std::mutex queue_mutex_;
            std::condition_variable queue_lock_condition_;
            
            std::string connect_address_;
            bool terminate_ = false;

            std::unordered_map<std::string, std::function<std::unique_ptr<google::protobuf::MessageLite> (const zmq::message_t&)> > proto_constructor_lookup_;

            std::future<void> zmq_requester_;
    };
};

template<class RequestType, class ReplyType>
std::future<std::unique_ptr<ReplyType> > zmq::ProtoRequester::SendRequest(const std::string& function_name, const RequestType& request){
    static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
    static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");

    RegisterProtoConstructor(RequestType::default_instance());
    RegisterProtoConstructor(ReplyType::default_instance());
    
    auto function_name_hash = zmq::GetRequestReplyFunction(function_name, RequestType::default_instance(), ReplyType::default_instance());
    auto future = EnqueueRequest(function_name_hash, request, ReplyType::default_instance());

    return std::async( std::launch::deferred, [](std::future<std::unique_ptr<google::protobuf::MessageLite> > future) {
        auto new_future = std::move(future);
        if(new_future.valid()){
            auto uptr = new_future.get();
            if(uptr){
                auto base_raw_ptr = uptr.release();
                return std::unique_ptr<ReplyType>(dynamic_cast<ReplyType*>(base_raw_ptr));
            }
        }
        throw std::runtime_error("DED");
    }, std::move(future));
};

#endif //RE_COMMON_ZMQ_PROTOREQUESTER_H