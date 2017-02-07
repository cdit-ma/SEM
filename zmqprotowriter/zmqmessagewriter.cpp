#include "zmqmessagewriter.h"
#include <iostream>

ZMQMessageWriter::ZMQMessageWriter(){
    context_ = new zmq::context_t();
    socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket_->setsockopt(ZMQ_SNDHWM, 10000);
}

ZMQMessageWriter::~ZMQMessageWriter(){
    Terminate();
    if(socket_){
        delete socket_;
    }

    if(context_){
        delete context_;
    }
}

bool ZMQMessageWriter::BindPublisherSocket(std::string endpoint){
    socket_->bind(endpoint.c_str());
    return true;
}

void ZMQMessageWriter::PushMessage(google::protobuf::MessageLite* message){
    std::string str;
    if(message && message->SerializeToString(&str)){
        PushString(&str);
    }
}

void ZMQMessageWriter::PushString(std::string *message){
    if(message){
        //Construct a zmq message
        zmq::message_t data(message->c_str(), message->size());
        //Send the zmq message
        socket_->send(data);
    }
}

void ZMQMessageWriter::Terminate(){
    //Do nothing
}
    