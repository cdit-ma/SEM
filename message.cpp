#include "message.h"

 Message::Message() : BaseMessage(0){
     //Construct a BaseMessage
 }

void Message::set_instName(const std::string val){
    this->instName_ = val;
}

void Message::set_content(const std::string val){
    this->content_ = val;
}

void Message::set_time(const int val){
    this->time_ = val;
}

std::string Message::content(){
    return content_;
}

std::string Message::instName(){
    return instName_;
}

int Message::time(){
    return time_;
}
