#include "zmqrxmessage.h"

//Includes the ::Message and proto::Message
#include "../proto/message/messageconvert.h"

//Include the templated InEventPort Implementation for ZMQ
#include "zmq/ineventport.hpp"

zmq::RxMessage::RxMessage(rxMessageInt* component, std::string end_point){
    //Store the component this port belongs too
    this->component_ = component;

    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(end_point);

    //Construct a concrete ZMQ InEventPort linked to callback into this.
    this->event_port_ = new zmq::InEventPort<::Message, proto::Message>(this, v);
}

void zmq::RxMessage::rxMessage(Message* message){
    //Call back into the component.
    component_->rxMessage(message);
}

void zmq::RxMessage::rx_(::Message* message){
    //Call back into the component.
    rxMessage(message);
}