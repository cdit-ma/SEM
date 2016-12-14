#include "zmqtxmessage.h"

//Includes the ::Message and proto::Message
#include "../proto/messageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmqouteventport.hpp"

zmq::TxMessage::TxMessage(txMessageInt* component, std::string end_point){
    this->component_ = component;
    
    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(end_point);

    //Construct a concrete ZMQ InEventPort linked to callback into this.
    this->event_port_ = new zmq::Zmq_OutEventPort<::Message, proto::Message>(this, v);
}

void zmq::TxMessage::txMessage(Message* message){
    //Call into the port
    event_port_->tx_(message);
}
