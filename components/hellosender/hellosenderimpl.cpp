#include "hellosenderimpl.h"
#include <iostream>

HelloSenderImpl::HelloSenderImpl(std::string name):HelloSenderInt(name){

};

void HelloSenderImpl::periodic_event(){
    Message* msg = new Message();
    msg->set_time(this->sentCount_ ++);
    msg->set_instName(this->instName());
    msg->set_content(this->message());
    HelloSenderInt::txMessage(msg);
}

Component* HelloSenderImpl::constructor_fn(std::string name){
    return new HelloSenderImpl(name);
}
void HelloSenderImpl::destructor_fn(Component* c){
    auto c2 = static_cast<HelloSenderImpl*>(c);
    if(c2){
        delete c2;
    }
}