#include "helloreceiverint.h"
#include <iostream>

HelloReceiverInt::HelloReceiverInt(std::string name): Component(name){
    {
        auto att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        AddAttribute(att);
    }

    //Add callback to callback into the rxMessage function
    AddCallback("rxMessage", [this](BaseMessage* m) {rxMessage((::Message*) m);});
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
