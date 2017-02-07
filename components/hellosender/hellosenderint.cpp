#include "hellosenderint.h"
#include <core/eventports/outeventport.hpp>


HelloSenderInt::HelloSenderInt(std::string name): Component(name){
    {
        auto att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        AddAttribute(att);
    }
    {
        auto att = new Attribute();
        att->name = "message";
        att->type = AT_STRING;
        AddAttribute(att);
    }

    AddCallback("tick", [this](BaseMessage* m) {tick();});
}

std::string HelloSenderInt::instName(){
    std::string str;
    auto a = GetAttribute("instName");
    if(a){
        str = a->get_string();
    }
    return str;
}

void HelloSenderInt::set_instName(const std::string val){
    auto a = GetAttribute("instName");
    if(a){
        a->set_string(val);
    }
}

std::string HelloSenderInt::message(){
    std::string str;
    auto a = GetAttribute("message");
    if(a){
        str = a->get_string();
    }
    return str;
}

void HelloSenderInt::set_message(const std::string val){
    auto a = GetAttribute("message");
    if(a){
        a->set_string(val);
    }
}

void HelloSenderInt::txMessage(::Message* message){
    auto a = GetEventPort("txMessage");
    if(a){
        auto b = (::OutEventPort<Message> *)(a);
        if(b){
            b->tx(message);
        }
    }
    delete message;
}
