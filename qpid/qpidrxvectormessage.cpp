#include "qpidrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated InEventPort Implementation for qpid
#include "qpid/ineventport.hpp"

qpid::RxVectorMessage::RxVectorMessage(rxVectorMessageInt* component, std::string end_point){
    //Store the component this port belongs too
    this->component_ = component;

    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(end_point);

    //Construct a concrete qpid InEventPort linked to callback into this.
    this->event_port_ = new qpid::InEventPort<::VectorMessage, cdit::VectorMessage>(this, v);
}

void qpid::RxVectorMessage::rxVectorMessage(VectorMessage* message){
    //Call back into the component.
    component_->rxVectorMessage(message);
}

void qpid::RxVectorMessage::rx_(::VectorMessage* message){
    //Call back into the component.
    rxVectorMessage(message);
}