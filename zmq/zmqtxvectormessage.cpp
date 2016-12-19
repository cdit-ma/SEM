#include "zmqtxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmq/outeventport.hpp"

zmq::TxVectorMessage::TxVectorMessage(txVectorMessageInt* component, std::string end_point){
    this->component_ = component;
    
    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(end_point);

    //Construct a concrete ZMQ InEventPort linked to callback into this.
    this->event_port_ = new zmq::OutEventPort<::VectorMessage, cdit::VectorMessage>(this, v);
}

void zmq::TxVectorMessage::txVectorMessage(::VectorMessage* message){
    //Call into the port
    event_port_->tx_(message);
}