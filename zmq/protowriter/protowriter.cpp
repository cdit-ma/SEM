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
 
#include "protowriter.h"
#include <iostream>
#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
#include <thread>
#include "monitor.h"

zmq::ProtoWriter::ProtoWriter(){
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t());
    socket_ = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_PUB));
    
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket_->setsockopt(ZMQ_SNDHWM, 10000);
}

zmq::ProtoWriter::~ProtoWriter(){
    Terminate();
}

void zmq::ProtoWriter::AttachMonitor(std::unique_ptr<zmq::Monitor> monitor, const int event_type){
    std::unique_lock<std::mutex> lock(mutex_);
    //TODO: re-add GetNewMonitorAddress()
    std::string addr = "inproc://Monitor_" + std::to_string(monitors_.size());
    monitor->MonitorSocket(*socket_, addr, event_type);
    monitors_.emplace_back(std::move(monitor));
}

bool zmq::ProtoWriter::BindPublisherSocket(const std::string& endpoint){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);
    if(socket_){
        try{
            socket_->bind(endpoint.c_str());    
        }catch(zmq::error_t &ex){
            std::cerr << "zmq::ProtoWriter::BindPublisherSocket(" << endpoint << "): " << ex.what() << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

int zmq::ProtoWriter::GetTxCount(){
    std::unique_lock<std::mutex> lock(mutex_);
    return tx_count_;
}

bool zmq::ProtoWriter::PushMessage(const std::string& topic, google::protobuf::MessageLite* message){
    bool success = false;
    if(message){
        std::string message_str;
        if(message->SerializeToString(&message_str)){
           success = PushString(topic, message->GetTypeName(), message_str);
        }
        delete message;
    }
    return success;
}

bool zmq::ProtoWriter::PushMessage(google::protobuf::MessageLite* message){
    return PushMessage("", message);
}

bool zmq::ProtoWriter::PushString(const std::string& topic, const std::string& type, const std::string& message){
    std::unique_lock<std::mutex> lock(mutex_);
    if(socket_){
        //Construct a zmq message for both the type and message data
        zmq::message_t topic_data(topic.begin(), topic.end());
        zmq::message_t type_data(type.begin(), type.end());
        zmq::message_t message_data(message.begin(), message.end());
        
        //Send Type then Data
        try{
            socket_->send(topic_data, ZMQ_SNDMORE);
            socket_->send(type_data, ZMQ_SNDMORE);
            socket_->send(message_data);
            tx_count_ ++;
            return true;   
        }catch(zmq::error_t &ex){
            std::cerr << "zmq::ProtoWriter::PushString(): " << ex.what() << std::endl;
        }
    }
    return false;
}

void zmq::ProtoWriter::Terminate(){
    std::unique_lock<std::mutex> lock(mutex_);
    //Teardown socket
    socket_.reset();
    //Remove monitors
    monitors_.clear();
    //Teardown context
    context_.reset();
}
