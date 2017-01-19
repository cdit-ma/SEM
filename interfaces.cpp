#include "interfaces.h"
#include <iostream>



ProxyInt::ProxyInt(std::string name): Component(name){
};



void ProxyInt::txMessage(::Message* message){
    if(txMessage_){
        txMessage_->tx(message);
    }
};

void ProxyInt::_set_txMessage(::OutEventPort<::Message>* tx){
    if(tx){
        txMessage_ = tx;    
    }
}

void ProxyInt::_set_rxMessage(::InEventPort<::Message>* rx){
    if(rx){
        rxMessage_ = rx;    
    }
}


//ATTRIBUTE STUFF
std::string SenderInt::instName(){
    std::string str;
    auto a = get_attribute("instName");
    if(a){
        str = a->get_string();
    }
    return str;
};

void SenderInt::set_instName(const std::string val){
    auto a = get_attribute("instName");
    if(a){
        a->set_string(val);
    }
};

std::string SenderInt::message(){
    std::string str;
    auto a = get_attribute("message");
    if(a){
        str = a->get_string();
    }
    return str;
};

void SenderInt::set_message(const std::string val){
    auto a = get_attribute("message");
    if(a){
        a->set_string(val);
    }
};

SenderInt::SenderInt(std::string name): Component(name){
    //Add attributes
    {
        Attribute* att = new Attribute();
        att->name = "instName";
        att->type = AT_STRING;
        add_attribute(att);
    }
    {
        Attribute* att = new Attribute();
        att->name = "message";
        att->type = AT_STRING;
        add_attribute(att);
    }
};

void SenderInt::_set_txMessage(::OutEventPort<::Message>* tx){
    if(tx){
        txMessage_ = tx;    
    }
}

void SenderInt::_set_txVectorMessage(::OutEventPort<::VectorMessage>* tx){
    if(tx){
        txVectorMessage_ = tx;    
    }
}



void SenderInt::txMessage(::Message* message){
    if(txMessage_){
        txMessage_->tx(message);
    }
};

void SenderInt::txVectorMessage(::VectorMessage* message){
    if(txVectorMessage_){
        txVectorMessage_->tx(message);
    }
};



ReceiverInt::ReceiverInt(std::string name): Component(name){};

std::string ReceiverInt::instName(){
    return instName_;
};

void ReceiverInt::set_instName(const std::string val){
    this->instName_ = val;
};


void ReceiverInt::_set_rxMessage(::InEventPort<::Message>* rx){
    if(rx){
        rxMessage_ = rx;    
    }
};

void ReceiverInt::_set_rxVectorMessage(::InEventPort<::VectorMessage>* rx){
    if(rx){
        rxVectorMessage_ = rx;    
    }
};



