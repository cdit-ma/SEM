#include "qpidtxmessage.h"
#include "qpid/outeventport.hpp"

qpid::TxMessage::TxMessage(txMessageInt* component, std::string broker, std::string topic){
    this->component_ = component;

    this->event_port_ = new qpid::OutEventPort<::Message, proto::Message>(this, broker, topic);
}

void qpid::TxMessage::txMessage(Message* message){
    event_port_->tx_(message);
}