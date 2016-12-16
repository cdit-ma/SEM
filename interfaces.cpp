#include "interfaces.h"

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

