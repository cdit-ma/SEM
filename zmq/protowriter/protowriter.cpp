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
#include <future>
#include <functional>

zmq::ProtoWriter::ProtoWriter():
    message_process_delay_(200)
{
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    socket_ = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_PUB));
    
    //Linger until all messages are sent to a peer
    socket_->setsockopt(ZMQ_LINGER, -1);
    //Set an unlimited sending highwater mark
    socket_->setsockopt(ZMQ_SNDHWM, 0);
}

zmq::ProtoWriter::~ProtoWriter(){
    Terminate();
    std::cout << "* zmq::ProtoWriter: Wrote "  << GetTxCount() << " messages." << std::endl;
}

void zmq::ProtoWriter::AttachMonitor(std::unique_ptr<zmq::Monitor> monitor, const int event_type){
    std::unique_lock<std::mutex> lock(mutex_);
    monitor_futures_.emplace_back(std::async(std::launch::async, &Monitor::MonitorThread, monitor.get(), std::ref(*socket_), event_type));
    monitors_.emplace_back(std::move(monitor));
}

bool zmq::ProtoWriter::BindPublisherSocket(const std::string& endpoint){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);
    if(socket_){
        try{
            socket_->bind(endpoint.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

bool zmq::ProtoWriter::PushMessage(const std::string& topic, std::unique_ptr<google::protobuf::MessageLite> message){
    bool success = false;
    if(message){
        std::string message_str;
        if(message->SerializeToString(&message_str)){
           success = PushString(topic, message->GetTypeName(), message_str);
        }
    }
    return success;
}

bool zmq::ProtoWriter::PushMessage(std::unique_ptr<google::protobuf::MessageLite> message){
    return PushMessage("", std::move(message));
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
            UpdateBackpressure(true);
            return true;
        }catch(zmq::error_t &ex){
            std::cerr << "zmq::ProtoWriter::PushString(): " << ex.what() << std::endl;
        }
    }
    return false;
}

void zmq::ProtoWriter::UpdateBackpressure(bool increment_sender){
    auto now = std::chrono::steady_clock::now();
    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(now - backpressure_update_time_);
    
    bool initial_message = tx_count_ == 0;
    //Can send 
    if(initial_message){
        backpressure_update_time_ = now;
    }
    
    if(increment_sender){
        backpressure_ ++;
        tx_count_ ++;
    }

    if(!initial_message){
        int64_t message_count = duration_us / message_process_delay_;
        if(message_count > 0){
            backpressure_ -= message_count;
            if(backpressure_ < 0){
                backpressure_ = 0;
            }
            backpressure_update_time_ = now;
        }
    }
}

void zmq::ProtoWriter::Terminate(){
    std::unique_lock<std::mutex> lock(mutex_);
    UpdateBackpressure(false);

    if(backpressure_ > 0){
        std::chrono::microseconds sleep_us(message_process_delay_ * backpressure_);
        auto sleep_ms = std::chrono::duration_cast<std::chrono::milliseconds>(sleep_us);
        std::cout << "* Sleeping for: " << sleep_ms.count() << " ms to free backpressure on ProtoWriter." << std::endl;
        std::this_thread::sleep_for(sleep_us);
    }

    //Teardown socket
    socket_.reset();

    //Remove monitors
    monitors_.clear();
    //Teardown context
    context_.reset();
}
