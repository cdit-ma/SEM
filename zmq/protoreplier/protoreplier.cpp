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

}

zmq::ProtoReplier::~ProtoReplier(){

}

void zmq::ProtoReplier::Bind(const std::string& address){

}

void zmq::ProtoReplier::Start(){

}

void zmq::ProtoReplier::Terminate(){

}

void zmq::ProtoReplier::RegisterNewProto(const google::protobuf::MessageLite& request_default_instance, const google::protobuf::MessageLite& reply_default_instance, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> callback_function){

}