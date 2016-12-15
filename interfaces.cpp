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


void SenderInt::txMessage(Message* message){
    if(txMessageInt_){
        txMessageInt_->txMessage(message);
    }
};

void txMessage(Message* message);

std::string ReceiverInt::instName(){
    return instName_;
};

void ReceiverInt::set_instName(const std::string val){
    this->instName_ = val;
};


void ReceiverInt::_set_rxMessage(rxMessageInt* port){
    this->rxMessageInt_ = port;
};