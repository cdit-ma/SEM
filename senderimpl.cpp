#include "senderimpl.h"
#include <iostream>
int i = 0;
void SenderImpl::periodic_event(){
    Message* msg = new Message();
    msg->set_content("Test");
    msg->set_instName("Poop");
    msg->set_time(i++);
    std::cout << "SenderImpl::periodic_event(): Message: " << msg->time() << std::endl;
    txMessage(msg);
}

void SenderImpl::txMessage(Message* message){
    this->txMessage_->txMessage(message);
}
