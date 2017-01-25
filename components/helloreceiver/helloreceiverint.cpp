#include "helloreceiverint.h"
#include <iostream>

HelloReceiverInt::HelloReceiverInt(std::string name): Component(name){
    {
        auto att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        add_attribute(att);
    }

    //Add callback
    add_callback("rxMessage", std::bind(&HelloReceiverInt::_rxMessage_, this, std::placeholders::_1));
}

std::string HelloReceiverInt::instName(){
    std::string str;
    auto a = get_attribute("instName");
    if(a){
        str = a->get_string();
    }
    return str;
}

void HelloReceiverInt::set_instName(const std::string val){
    auto a = get_attribute("instName");
    if(a){
        a->set_string(val);
    }
}

void HelloReceiverInt::_rxMessage_(BaseMessage* _m){
    Message* message = static_cast<Message*>(_m);
    if(message){
        rxMessage(message);
    }
};
/*
void HelloReceiverInt::_set_rxMessage(::InEventPort<::Message>* rx){
    if(rx){
        rxMessage_ = rx;    
    }
}*/

