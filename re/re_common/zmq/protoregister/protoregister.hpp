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
 
#ifndef RE_COMMON_ZMQ_PROTOREGISTER_H
#define RE_COMMON_ZMQ_PROTOREGISTER_H

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <zmq.hpp>
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
#include <google/protobuf/message_lite.h>
#ifdef _WIN32
#pragma warning(pop)
#endif

namespace zmq{
    class ProtoRegister{
        public:
            template<class ProtoType>
            void RegisterProto();
            std::unique_ptr<google::protobuf::MessageLite> ConstructProto(const std::string& type_name, const zmq::message_t& data);
            std::unique_ptr<google::protobuf::MessageLite> ConstructProto(const std::string& type_name, const std::string& data);
        private:
            void RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance);
            std::unique_ptr<google::protobuf::MessageLite> ConstructProto(const std::string& type_name, const void* data, int size);
            
            std::mutex mutex_;
            std::unordered_map<std::string, std::function<std::unique_ptr<google::protobuf::MessageLite> (const void*, int)> > proto_lookup_;
    };
};

template<class ProtoType>
void zmq::ProtoRegister::RegisterProto(){
    static_assert(std::is_base_of<google::protobuf::MessageLite, ProtoType>::value, "ProtoType must inherit from google::protobuf::MessageLite");
    RegisterProtoConstructor(ProtoType::default_instance());
};

#endif //RE_COMMON_ZMQ_PROTOREGISTER_H
