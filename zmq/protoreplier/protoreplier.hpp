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
 
#ifndef RE_COMMON_ZMQ_PROTOREPLIER_H
#define RE_COMMON_ZMQ_PROTOREPLIER_H

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <set>
#include <future>

#include <zmq.hpp>
#include <google/protobuf/message_lite.h>

namespace zmq{
    class ProtoReplier{
        public:
            ProtoReplier();
            ~ProtoReplier();

            void Bind(const std::string& address);
            
            void Start();
            void Terminate();

            template<class RequestType, class ReplyType>
            void RegisterProtoCallback(const std::string& function_name, std::function<std::unique_ptr<ReplyType>(const RequestType&)> fn);

            std::unique_ptr<google::protobuf::MessageLite> ConstructPB(const std::string& type);
        private:
            void RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance);

            void RegisterNewProto(const std::string& function_name, const google::protobuf::MessageLite& request_default_instance, const google::protobuf::MessageLite& reply_default_instance, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> callback_function);
            
            void ZmqReplier();

            std::unique_ptr<zmq::context_t> context_;
            
            std::set<std::string> bind_addresses_;

            std::unordered_map<std::string, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> > callback_lookup_;
            std::unordered_map<std::string, std::function<std::unique_ptr<google::protobuf::MessageLite> (const zmq::message_t&)> > proto_constructor_lookup_;

            std::future<void> zmq_replier_;
    };
};

template<class RequestType, class ReplyType>
void zmq::ProtoReplier::RegisterProtoCallback(const std::string& function_name, std::function<std::unique_ptr<ReplyType>(const RequestType&)> callback_function){
    static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
    static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");
    
    RegisterNewProto(function_name, RequestType::default_instance(), ReplyType::default_instance(), [callback_function](const google::protobuf::MessageLite& request){
            return std::move(callback_function((const RequestType&) request));
    });
}

#endif //RE_COMMON_ZMQ_PROTOREPLIER_H