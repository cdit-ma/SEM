#include "hellosenderint.h"

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

    //Add to call back
    add_callback("tick", std::bind(&HelloSenderInt::_periodic_event_, this, std::placeholders::_1));
}

void HelloSenderInt::_periodic_event_(BaseMessage* message){
    
    this->periodic_event();
    delete message;
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
        auto b = static_cast<::OutEventPort<Message> *>(a);
        if(b){
            b->tx(message);
        }
    }
    delete message;
}
