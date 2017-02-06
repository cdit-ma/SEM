#include "zmqmessagewriter.h"
#include <iostream>
#include "systemstatus.pb.h"
#include <chrono>
#include <thread>
ZMQMessageWriter::ZMQMessageWriter(){
    context_ = new zmq::context_t(1);

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
        zmq::message_t data(str.c_str(), str.size());
        return socket_->send(data);
    }
    return false;
}

bool ZMQMessageWriter::Terminate(){
    //Do nothing
    return true;
}
    