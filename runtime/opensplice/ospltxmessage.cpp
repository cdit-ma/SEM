#include "ospltxmessage.h"

#include "message_DCPS.hpp"

#include "opensplice/outeventport.hpp"

ospl::TxMessage::TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string  writer_name, std::string  topic_name){
    this->component_ = component;
    this->event_port_ = new ospl::OutEventPort<::Message, cdit::Message>(this, domain_id, publisher_name, writer_name, topic_name);
}

void ospl::TxMessage::txMessage(::Message* message){
    //Call into the port
    event_port_->tx_(message);
}
