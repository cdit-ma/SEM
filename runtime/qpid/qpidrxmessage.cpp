#include "qpidrxmessage.h"
#include "qpid/ineventport.hpp"


qpid::RxMessage::RxMessage(rxMessageInt* component, std::string broker, std::string topic){
    //Store the component this port belongs too
    this->component_ = component;

    //Construct a concrete ZMQ InEventPort linked to callback into this.
    this->event_port_ = new qpid::InEventPort<::Message, proto::Message>(this, broker, topic);
}

void qpid::RxMessage::rxMessage(Message* message){
    //Call back into the component.
    component_->rxMessage(message);
}

void qpid::RxMessage::rx_(::Message* message){
    //Call back into the component.
    rxMessage(message);
}