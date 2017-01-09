#include "interfaces.h"
#include <iostream>
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

ReceiverInt::ReceiverInt(std::string name): Component(name){
};

void SenderInt::activate(){
    std::cout << "Activate Sender " << get_name() << std::endl;

    if(txVectorMessageInt_){
        //txVectorMessageInt_->activate();
    }
        //Activate
    if(txMessageInt_){
        //txMessageInt_->activate();
    }

        //Activate
};

void SenderInt::passivate(){
    std::cout << "Passivate Sender " << get_name() << std::endl;
    if(txVectorMessageInt_){}
        //Activate
    if(txMessageInt_){}
        //Activate
};

void SenderInt::_set_txMessage(txMessageInt* port){
    this->txMessageInt_ = port;
};

void SenderInt::_set_txVectorMessage(txVectorMessageInt* port){
    this->txVectorMessageInt_ = port;
};

void SenderInt::txMessage(::Message* message){
    if(txMessageInt_){
        txMessageInt_->txMessage(message);
    }
};

void SenderInt::txVectorMessage(::VectorMessage* message){
    if(txMessageInt_){
        txVectorMessageInt_->txVectorMessage(message);
    }
};


std::string ReceiverInt::instName(){
    return instName_;
};

void ReceiverInt::set_instName(const std::string val){
    this->instName_ = val;
};


void ReceiverInt::_set_rxMessage(rxMessageInt* port){
    this->rxMessageInt_ = port;
};

void ReceiverInt::_set_rxVectorMessage(rxVectorMessageInt* port){
    this->rxVectorMessageInt_ = port;
};


void ReceiverInt::activate(){
    std::cout << "Activate Sender " << get_name() << std::endl;

    if(rxVectorMessageInt_){
        rxVectorMessageInt_->activate();
    }
    if(rxMessageInt_){
        rxMessageInt_->activate();
    }
};

void ReceiverInt::passivate(){
    std::cout << "Passivate Sender " << get_name() << std::endl;
      if(rxVectorMessageInt_){
        rxVectorMessageInt_->passivate();
    }
    if(rxMessageInt_){
        rxMessageInt_->passivate();
    }
};
