#include "zmqrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated InEventPort Implementation for ZMQ
#include "zmq/ineventport.hpp"

zmq::RxVectorMessage::RxVectorMessage(rxVectorMessageInt* component, std::string end_point){
    //Store the component this port belongs too
    this->component_ = component;

    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(end_point);

    //Construct a concrete ZMQ InEventPort linked to callback into this.
    this->event_port_ = new zmq::Zmq_InEventPort<::VectorMessage, cdit::VectorMessage>(this, v);
}

void zmq::RxVectorMessage::rxVectorMessage(VectorMessage* message){
    //Call back into the component.
    component_->rxVectorMessage(message);
}

void zmq::RxVectorMessage::rx_(::VectorMessage* message){
    //Call back into the component.
    rxVectorMessage(message);
}