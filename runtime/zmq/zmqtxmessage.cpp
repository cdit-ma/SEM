#include "zmqtxmessage.h"
#include <iostream>

zmq_txMessage::zmq_txMessage(txMessageInt* component, zmq::context_t* context, std::string endpoint){
    this->component_ = component;
    this->context_ = context;
    //Construct a socket!
    this->socket_ = new zmq::socket_t(*context, ZMQ_PUB);
    this->socket_->bind(endpoint.c_str());
}


void zmq_txMessage::txMessage(Message* message){
    std::string str;
    auto m = proto::message_to_proto(message);
     

    if(m->SerializeToString(&str)){
        zmq::message_t topic("A", 1);
        zmq::message_t topic2("B", 1);
        //Construct a message and send it
        zmq::message_t data(str.c_str(), str.size());
        zmq::message_t data2(str.c_str(), str.size());
        socket_->send(topic, ZMQ_SNDMORE);
        socket_->send(data);

        socket_->send(topic2, ZMQ_SNDMORE);
        socket_->send(data2);
    }
}