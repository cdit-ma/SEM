#include "rtirxmessage.h"

#include "message.hpp"

//Include the templated OutEventPort Implementation for OSPL
#include "rti/ineventport.hpp"

rti::RxMessage::RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name,std::string reader_name, std::string  topic_name){
    this->component_ = component;

     //Construct a concrete Ospl InEventPort linked to callback into this.
    this->event_port_ = new rti::InEventPort<::Message, rti::cdit::Message>(this, domain_id, subscriber_name, reader_name, topic_name);
}

void rti::RxMessage::rxMessage(::Message* message){
    //Call back into the component.
    component_->rxMessage(message);
}

void rti::RxMessage::rx_(::Message* message){
    //Call back into the component.
    rxMessage(message);
}