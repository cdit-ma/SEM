#include "hellosenderint.h"
#include <core/eventports/outeventport.hpp>


HelloSenderInt::HelloSenderInt(std::string name): Component(name){
    {
        auto att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        add_attribute(att);
    }
    {
        auto att = new Attribute();
        att->name = "message";
        att->type = AT_STRING;
        add_attribute(att);
    }

    add_callback("tick", [this](BaseMessage* m) {tick();delete m;});
}

std::string HelloSenderInt::instName(){
    std::string str;
    auto a = get_attribute("instName");
    if(a){
        str = a->get_string();
    }
    return str;
}

void HelloSenderInt::set_instName(const std::string val){
    auto a = get_attribute("instName");
    if(a){
        a->set_string(val);
    }
}

std::string HelloSenderInt::message(){
    std::string str;
    auto a = get_attribute("message");
    if(a){
        str = a->get_string();
    }
    return str;
}

void HelloSenderInt::set_message(const std::string val){
    auto a = get_attribute("message");
    if(a){
        a->set_string(val);
    }
}

void HelloSenderInt::txMessage(::Message* message){
    auto a = get_event_port("txMessage");
    if(a){
        auto b = (::OutEventPort<Message> *)(a);
        if(b){
            b->tx(message);
        }
    }
    delete message;
}
