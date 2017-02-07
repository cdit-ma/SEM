#include "helloreceiverint.h"
#include <iostream>

HelloReceiverInt::HelloReceiverInt(std::string name): Component(name){
    {
        auto att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        AddAttribute(att);
    }

    //Add callback
    AddCallback("rxMessage", std::bind(&HelloReceiverInt::_rxMessage_, this, std::placeholders::_1));
}

std::string HelloReceiverInt::instName(){
    std::string str;
    auto a = GetAttribute("instName");
    if(a){
        str = a->get_string();
    }
    return str;
}

void HelloReceiverInt::set_instName(const std::string val){
    auto a = GetAttribute("instName");
    if(a){
        a->set_string(val);
    }
}

void HelloReceiverInt::_rxMessage_(BaseMessage* m){
    if(m){
        //HARD CAST
        rxMessage((::Message*) m);
    }
};

