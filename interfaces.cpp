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

std::string RecieverInt::instName(){
    return instName_;
};

void RecieverInt::set_instName(const std::string val){
    this->instName_ = val;
};