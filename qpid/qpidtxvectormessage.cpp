#include "qpidtxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for qpid
#include "qpid/outeventport.hpp"

qpid::TxVectorMessage::TxVectorMessage(txVectorMessageInt* component, std::string broker, std::string topic){
    this->component_ = component;

    //Construct a concrete qpid InEventPort linked to callback into this.
    this->event_port_ = new qpid::OutEventPort<::VectorMessage, cdit::VectorMessage>(this, broker, topic);
}

void qpid::TxVectorMessage::txVectorMessage(::VectorMessage* message){
    //Call into the port
    event_port_->tx_(message);
}