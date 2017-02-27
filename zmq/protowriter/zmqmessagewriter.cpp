#include "zmqmessagewriter.h"
#include <iostream>
#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
ZMQMessageWriter::ZMQMessageWriter(){
    context_ = new zmq::context_t();
    socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket_->setsockopt(ZMQ_SNDHWM, 10000);
}

ZMQMessageWriter::~ZMQMessageWriter(){
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

bool ZMQMessageWriter::BindPublisherSocket(std::string endpoint){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);
    if(socket_){
        socket_->bind(endpoint.c_str());    
        return true;
    }
    return false;
}

void ZMQMessageWriter::PushMessage(google::protobuf::MessageLite* message){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);

    std::string type_name;
    std::string str;
    if(message){
        if(message->SerializeToString(&str)){
            type_name = message->GetTypeName();
            PushString(&type_name, &str);
        }
        delete message;
    }
}

void ZMQMessageWriter::PushString(std::string * type, std::string *message){
    if(message){
        //Construct a zmq message for both the type and message data
        zmq::message_t type_data(type->c_str(), type->size());
        zmq::message_t message_data(message->c_str(), message->size());
        
        //Send Type then Data
        socket_->send(type_data, ZMQ_SNDMORE);
        socket_->send(message_data);
    }
}

void ZMQMessageWriter::Terminate(){
    //Do nothing
}
    