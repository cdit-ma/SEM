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

void zmq::ProtoWriter::RegisterMonitorCallback(const uint8_t& event, std::function<void(int, std::string)> fn){
    if(!monitor_){
        monitor_ = std::unique_ptr<zmq::Monitor>(new zmq::Monitor(*socket_));
    }
    monitor_->RegisterEventCallback(event, fn);
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

uint64_t zmq::ProtoWriter::GetTxCount() const{
    return tx_count_;
}

// REVIEW (Mitch): [[nodiscard]]?
// REVIEW (Mitch): Follow the bools all the way down and fix...
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
        ++backpressure_;
        ++tx_count_;
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

    auto back = backpressure_.load();
    if(back > 0){
        std::chrono::microseconds sleep_us(message_process_delay_ * back);
        auto sleep_ms = std::chrono::duration_cast<std::chrono::milliseconds>(sleep_us);
        std::cout << "* Sleeping for: " << sleep_ms.count() << " ms to free backpressure on ProtoWriter." << std::endl;
        std::this_thread::sleep_for(sleep_us);
    }
    
    monitor_.reset();
    socket_.reset();
    context_.reset();
}
