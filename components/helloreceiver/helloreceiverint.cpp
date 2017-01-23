#include "helloreceiverint.h"
#include <iostream>

HelloReceiverInt::HelloReceiverInt(std::string name): Component(name){
    {
        auto att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        add_attribute(att);
    }
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

void HelloReceiverInt::_set_rxMessage(::InEventPort<::Message>* rx){
    if(rx){
        rxMessage_ = rx;    
    }
}

