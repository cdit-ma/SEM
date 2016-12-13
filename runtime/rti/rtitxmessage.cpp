#include "rtitxmessage.h"

#include "message.hpp"

#include "rtiouteventport.hpp"

rti::TxMessage::TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string  writer_name, std::string  topic_name){
    this->component_ = component;
    this->event_port_ = new rti::OutEventPort<::Message, rti::Message>(this, domain_id, publisher_name, writer_name, topic_name);
}

void rti::TxMessage::txMessage(::Message* message){
    //Call into the port
    event_port_->tx_(message);
}
