#include "senderimpl.h"
#include <iostream>
int i = 0;
void SenderImpl::periodic_event(){
    Message* msg = new Message();
    msg->set_time(this->sentCount_ ++);
    msg->set_instName(this->instName());
    msg->set_content(this->message());
    txMessage(msg);
}

void SenderImpl::txMessage(Message* message){
    if(txMessage_){
        this->txMessage_->txMessage(message);
    }
}
