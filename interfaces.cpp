#include "interfaces.h"
#include <iostream>


//ATTRIBUTE STUFF
std::string SenderInt::instName(){
    return instName_;
};

void SenderInt::set_instName(const std::string val){
    this->instName_ = val;
};

std::string SenderInt::message(){
    return message_;
};

void SenderInt::set_message(const std::string val){
    this->message_ = val;
};

SenderInt::SenderInt(std::string name): Component(name){
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



