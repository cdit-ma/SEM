#include "protowriter.h"
#include <iostream>
#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
#include <thread>
#include "../monitor/monitor.h"

zmq::ProtoWriter::ProtoWriter(){
    context_ = new zmq::context_t();
    socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket_->setsockopt(ZMQ_SNDHWM, 10000);
}

zmq::ProtoWriter::~ProtoWriter(){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);

    Terminate();

    if(socket_){
        delete socket_;
    }

    if(context_){
        delete context_;
    }
}

void zmq::ProtoWriter::AttachMonitor(zmq::Monitor* monitor, int event_type){
    if(monitor && socket_){
        //Attach monitor; using a new address
        monitor->MonitorSocket(socket_, GetNewMonitorAddress(), event_type);
    }
}

bool zmq::ProtoWriter::BindPublisherSocket(std::string endpoint){
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

void zmq::ProtoWriter::PushMessage(std::string* topic, google::protobuf::MessageLite* message){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);

    std::string type_name;
    std::string str;
    if(message){
        if(message->SerializeToString(&str)){
            type_name = message->GetTypeName();
            PushString(topic, &type_name, &str);
        }
        delete message;
    }
}

void zmq::ProtoWriter::PushString(std::string* topic, std::string* type, std::string* message){
    if(message){
        //Construct a zmq message for both the type and message data
        zmq::message_t topic_data(topic->c_str(), topic->size());
        zmq::message_t type_data(type->c_str(), type->size());
        zmq::message_t message_data(message->c_str(), message->size());
        
        //Send Type then Data
        socket_->send(topic_data, ZMQ_SNDMORE);
        socket_->send(type_data, ZMQ_SNDMORE);
        socket_->send(message_data);
    }
}

void zmq::ProtoWriter::Terminate(){
    //Do nothing
}
