#include "qpidrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated InEventPort Implementation for qpid
#include "qpid/ineventport.hpp"

qpid::RxVectorMessage::RxVectorMessage(rxVectorMessageInt* component, std::string broker, std::string topic){
    //Store the component this port belongs too
    this->component_ = component;


    //Construct a concrete qpid InEventPort linked to callback into this.
    this->event_port_ = new qpid::InEventPort<::VectorMessage, cdit::VectorMessage>(this, broker, topic);
}

void qpid::RxVectorMessage::rxVectorMessage(VectorMessage* message){
    //Call back into the component.
    component_->rxVectorMessage(message);
}

void qpid::RxVectorMessage::rx_(::VectorMessage* message){
    //Call back into the component.
    rxVectorMessage(message);
}