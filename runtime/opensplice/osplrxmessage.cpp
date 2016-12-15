#include "osplrxmessage.h"

#include "message_DCPS.hpp"

//Include the templated OutEventPort Implementation for OSPL
#include "opensplice/ineventport.hpp"



ospl::RxMessage::RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name, std::string topic_name){
    this->component_ = component;
     //Construct a concrete Ospl InEventPort linked to callback into this.
    this->event_port_ = new ospl::InEventPort<::Message, cdit::Message>(this, domain_id, subscriber_name, topic_name);
}

void ospl::RxMessage::rxMessage(::Message* message){
    //Call back into the component.
    component_->rxMessage(message);
}

void ospl::RxMessage::rx_(::Message* message){
    //Call back into the component.
    rxMessage(message);
}