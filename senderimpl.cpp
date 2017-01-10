#include "senderimpl.h"
#include <iostream>

SenderImpl::SenderImpl(std::string name) : SenderInt(name){
    //Add Periodic Event
    add_event_port(new PeriodicEvent(std::function<void(void)>(std::bind(&SenderImpl::periodic_event, this)), 1000));
};

void SenderImpl::periodic_event(){
    Message* msg = new Message();
    msg->set_time(this->sentCount_ ++);
    msg->set_instName(this->instName());
    msg->set_content(this->message());
    txMessage(msg);
}

void SenderImpl::periodic_event_v(){
    VectorMessage* msg = new VectorMessage();
    msg->dataName(this->instName());

    sentCount_ ++;
    for(int i = 0; i< 10; i++){
        msg->data_ptr().push_back(sentCount_ * i);
    }

    //msg->data(v);

    txVectorMessage(msg);
}