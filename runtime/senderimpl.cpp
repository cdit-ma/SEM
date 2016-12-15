#include "senderimpl.h"
#include <iostream>

void SenderImpl::periodic_event(){
    Message* msg = new Message();
    msg->set_time(this->sentCount_ ++);
    msg->set_instName(this->instName());
    msg->set_content(this->message());
    txMessage(msg);
}