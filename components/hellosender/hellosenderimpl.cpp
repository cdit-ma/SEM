#include "hellosenderimpl.h"
#include <iostream>

HelloSenderImpl::HelloSenderImpl(std::string name) : HelloSenderInt(name){

};

void HelloSenderImpl::tick(){
    {
        Message* msg = new Message();
        msg->set_time(this->sentCount_ ++);
        msg->set_instName(this->instName());
        msg->set_content(this->message());
        txMessage(msg);
        delete msg;
    }
}