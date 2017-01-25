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
    add_callback("periodic_event", std::bind(&HelloSenderInt::_periodic_event_, this, std::placeholders::_1));
}

void HelloSenderInt::_periodic_event_(BaseMessage* message){
    delete message;
    this->periodic_event();
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

void HelloSenderInt::_set_txMessage(::OutEventPort<::Message>* tx){
    if(tx){
        txMessage_ = tx;    
    }
}

void HelloSenderInt::txMessage(::Message* message){
    if(txMessage_){
        txMessage_->tx(message);
    }
    delete message;
}
