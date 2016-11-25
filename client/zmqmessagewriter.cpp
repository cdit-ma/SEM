#include "zmqmessagewriter.h"
#include <iostream>
#include "systemstatus.pb.h"
#include <chrono>
#include <thread>
ZMQMessageWriter::ZMQMessageWriter(){
    context = new zmq::context_t(1);

    socket = new zmq::socket_t(*context, ZMQ_PUB);
    
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket->setsockopt(ZMQ_SNDHWM, 10000);
    //socket->setsockopt(ZMQ_LINGER, 10000);
}

ZMQMessageWriter::~ZMQMessageWriter(){
    terminate();
    delete socket;
    delete context;
}

bool ZMQMessageWriter::bind_publisher_socket(std::string endpoint){
    socket->bind(endpoint.c_str());
    return true;
}

bool ZMQMessageWriter::push_message(google::protobuf::MessageLite* message){
    std::string str;

    
    if(message->SerializeToString(&str)){
        zmq::message_t data(str.c_str(), str.size());
        return socket->send(data);
    }
    return false;
}

bool ZMQMessageWriter::terminate(){
    //Do nothing
    return true;
}
    