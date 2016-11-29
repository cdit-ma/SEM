#include "zmqtxmessage.h"
#include <iostream>

zmq_txMessage::zmq_txMessage(txMessageInt* component, zmq::context_t* context, std::string endpoint){
    this->component_ = component;
    this->context_ = context;
    //Construct a socket!
    this->socket_ = new zmq::socket_t(*context, ZMQ_PUB);
    this->socket_->bind(endpoint);
}


void txMessage(Message* message){
    std::string str;
    auto m = message_to_proto(message);
     

    if(m->SerializeToString(&str)){
        //Construct a message and send it
        std::cout << "Sending :"  << str << std::endl;
        zmq::message_t data(str.c_str(), str.size());
    }
}