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

void SenderInt::_set_rxMessage(txMessageInt* port){
    this->txMessageInt_ = port;
};


void SenderInt::txMessage(Message* message){
    if(txMessageInt_){
        txMessageInt_->txMessage(message);
    }
};

void txMessage(Message* message);

std::string RecieverInt::instName(){
    return instName_;
};

void RecieverInt::set_instName(const std::string val){
    this->instName_ = val;
};


void RecieverInt::_set_rxMessage(rxMessageInt* port){}
    this->rxMessageInt_ = port;
};