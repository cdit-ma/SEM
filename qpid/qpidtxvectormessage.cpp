#include "qpidtxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for qpid
#include "qpid/outeventport.hpp"

qpid::TxVectorMessage::TxVectorMessage(txVectorMessageInt* component, std::string end_point){
    this->component_ = component;
    
    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(end_point);

    //Construct a concrete qpid InEventPort linked to callback into this.
    this->event_port_ = new qpid::OutEventPort<::VectorMessage, cdit::VectorMessage>(this, v);
}

void qpid::TxVectorMessage::txVectorMessage(::VectorMessage* message){
    //Call into the port
    event_port_->tx_(message);
}