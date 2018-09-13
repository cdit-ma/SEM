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
#include "../zmqutils.hpp"

zmq::ProtoReplier::ProtoReplier(){
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
}

zmq::ProtoReplier::~ProtoReplier(){

}

void zmq::ProtoReplier::Bind(const std::string& address){
    bind_addresses_.emplace(address);
}

void zmq::ProtoReplier::Start(){
    if(!zmq_replier_.valid()){
        zmq_replier_ = std::async(std::launch::async, &zmq::ProtoReplier::ZmqReplier, this);
    }
}

void zmq::ProtoReplier::Terminate(){

}

void zmq::ProtoReplier::ZmqReplier(){
    zmq::socket_t socket(*context_, ZMQ_REP);

    //Connect to all nodes on our network
    for (const auto& address : bind_addresses_){
        try{
            socket.bind(address.c_str());
        }catch(const zmq::error_t& ex){
            std::cerr << "zmq::ProtoReplier: Failed to connect to address: '" << address << "' " << ex.what() << std::endl;
        }
    }
    
    while(true){
		try{
            zmq::message_t zmq_function_name;
            zmq::message_t zmq_request_typename;
            zmq::message_t zmq_request_data;

            //Recieve Our Message
            socket.recv(&zmq_function_name);
            socket.recv(&zmq_request_typename);
            socket.recv(&zmq_request_data);


            //Get the type of the protobuf message
            const auto& str_function_name = Zmq2String(zmq_function_name);
            const auto& str_request_typename = Zmq2String(zmq_request_typename);

            //std::cerr << "Calling: " << str_function_name  << std::endl;

            std::string error_str;
            bool got_request_constructor = proto_constructor_lookup_.count(str_request_typename) > 0;
            bool got_function = callback_lookup_.count(str_function_name) > 0;

            std::unique_ptr<google::protobuf::MessageLite> reply_proto;

            if(got_request_constructor && got_function){
                try{
                    //Construct the Request Protobuf message
                    auto request_type = proto_constructor_lookup_[str_request_typename](zmq_request_data);

                    //Run the function
                    reply_proto = callback_lookup_[str_function_name](*request_type);
                }catch(const std::exception& ex){
                    error_str = ex.what();
                }
            }else if(!got_function){
                error_str = "zmq::ProtoReplier: Doesn't have a registered function: '" + str_function_name + "'";
            }else if(!got_request_constructor){
                error_str = "zmq::ProtoReplier: Doesn't have a registered proto: '" + str_request_typename + "'";
            }

            //Construct a success flag message
            uint32_t success = (reply_proto != nullptr) ? 0 : 1;
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
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::ProtoReplier: ZMQ Exception: " << ex.what() << std::endl;
            }
            break;
        }
    }
}

void zmq::ProtoReplier::RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance){
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

void zmq::ProtoReplier::RegisterNewProto(const std::string& function_name, const google::protobuf::MessageLite& request_default_instance, const google::protobuf::MessageLite& reply_default_instance, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> callback_function){
    //Register the callbacks
    RegisterProtoConstructor(request_default_instance);
    RegisterProtoConstructor(reply_default_instance);

    //Get the registered function name
    const auto& hashed_function_name = GetRequestReplyFunction(function_name, request_default_instance, reply_default_instance);

    if(!callback_lookup_.count(hashed_function_name)){
        std::cerr << "Registered: " << hashed_function_name << std::endl;
        callback_lookup_[hashed_function_name] = callback_function;
    }else{
        throw std::invalid_argument("Request Type '" + hashed_function_name + "' Already Registered");
    }
}


std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoReplier::ConstructPB(const std::string& type_name){
    
    if(proto_constructor_lookup_.count(type_name)){
        zmq::message_t message;
        return proto_constructor_lookup_[type_name](message);
    }else{
        throw std::invalid_argument("Proto Type '" + type_name + "' Not Registered");
    }
}