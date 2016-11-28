#include "senderimpl.h"
#include <iostream>

void SenderImpl::periodic_event(){
    Message* msg = new Message();
    msg->set_content("Test");
    msg->set_instName("Poop");
    std::cout << msg->get_base_message_id() << std::endl;
    txMessage(msg);
}

void SenderImpl::txMessage(Message* message){
    this->txMessage_->txMessage(message);
}
