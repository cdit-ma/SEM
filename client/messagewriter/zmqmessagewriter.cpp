#include "zmqmessagewriter.h"
#include <iostream>
#include "systemstatus.pb.h"
#include <chrono>
#include <thread>
ZMQMessageWriter::ZMQMessageWriter(){
    context_ = new zmq::context_t();

    socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket_->setsockopt(ZMQ_SNDHWM, 10000);
}

ZMQMessageWriter::~ZMQMessageWriter(){
    Terminate();
    delete socket_;
    delete context_;
}

bool ZMQMessageWriter::BindPublisherSocket(std::string endpoint){
    socket_->bind(endpoint.c_str());
    return true;
}

bool ZMQMessageWriter::PushMessage(google::protobuf::MessageLite* message){
    std::string str;
    if(message->SerializeToString(&str)){
        return PushString(&str);
    }
    return false;
}

bool ZMQMessageWriter::PushString(std::string *message){
    if(message){
        zmq::message_t data(message->c_str(), message->size());
        return socket_->send(data);
    }
    return false;
}

bool ZMQMessageWriter::Terminate(){
    //Do nothing
    return true;
}
    