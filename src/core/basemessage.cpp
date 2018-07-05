#include "basemessage.h"
#include <mutex>
#include <iostream>

BaseMessage::BaseMessage(){
    //Static mutex-y bois
    static std::mutex mutex_;
    static int counter_ = 0;

    std::lock_guard<std::mutex> lock(mutex_);
    this->id_ = counter_++;
    //std::cerr << "\t\tCONSTRUCTOR: " << get_base_message_id() << " " << this << std::endl;
};

//BaseMessage::~BaseMessage(){
    //std::cerr << "\t\tDESTRUCTOR: " << get_base_message_id() << " " << this << std::endl;
//}


BaseMessage::BaseMessage(const BaseMessage& other)
: BaseMessage::BaseMessage(){
    //std::cerr << "Copy Constructc: " << &other << " " << other.get_base_message_id() << " INTO " << this << " " << get_base_message_id() << std::endl;
};

BaseMessage BaseMessage::operator=(const BaseMessage& other){
    //std::cerr << "Copy Assign: " << other.get_base_message_id() << " INTO " << get_base_message_id() << std::endl;
    return BaseMessage(other);
}

int BaseMessage::get_base_message_id() const{
    return id_;
};

BaseMessage::BaseMessage(BaseMessage&& other){
    id_ = other.id_;
    //std::cerr << "Move Construct: " " " << &other << " " << other.get_base_message_id() << " INTO " << get_base_message_id() << " " << this << std::endl;
}

BaseMessage& BaseMessage::operator=(BaseMessage&& other){
    id_ = other.id_;
    //std::cerr << "Move Assign: " " " << &other << " " << other.get_base_message_id() << " INTO " << get_base_message_id() << " " << this << std::endl;
    return *this;
}