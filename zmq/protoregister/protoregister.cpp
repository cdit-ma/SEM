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
 
#include "protoregister.h"

void zmq::ProtoRegister::RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance){
    const auto& type_name = default_instance.GetTypeName();

    std::lock_guard<std::mutex> lock(mutex_);
    if(!proto_lookup_.count(type_name)){
        //Insert a factory function which will construct a protobuf message from the array
        proto_lookup_[type_name] = [&default_instance](const void* data, int size){
            //Construct a Protobuf message
            auto proto_obj = std::unique_ptr<google::protobuf::MessageLite>(default_instance.New());

            if(proto_obj && proto_obj->ParseFromArray(data, size) == false){
                //If message failed to parse, reset the object
                proto_obj.reset();
            }
            return proto_obj;
        };
    }
}

std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoRegister::ConstructProto(const std::string& type_name, const zmq::message_t& data){
    return ConstructProto(type_name, data.data(), data.size());
}

std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoRegister::ConstructProto(const std::string& type_name, const std::string& data){
    return ConstructProto(type_name, data.c_str(), data.size());
}

std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoRegister::ConstructProto(const std::string& type_name, const void* data, int size){
    std::lock_guard<std::mutex> lock(mutex_);
    try{
        return proto_lookup_.at(type_name)(data, size);
    }catch(const std::out_of_range& exception){
        throw std::runtime_error("Proto Type '" + type_name + "' Not Registered!");
    }
}