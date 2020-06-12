#include "basemessage.h"
#include <mutex>
#include <iostream>
#include <mutex>
#include "id_generator.h"

BaseMessage::BaseMessage(){
    id_ = IdGenerator::get_next_id();
};

BaseMessage::BaseMessage(const BaseMessage& other)
: BaseMessage::BaseMessage(){
};

BaseMessage BaseMessage::operator=(const BaseMessage& other){
    return BaseMessage(other);
}

int BaseMessage::get_base_message_id() const{
    return id_;
};

BaseMessage::BaseMessage(BaseMessage&& other){
    id_ = other.id_;
}

BaseMessage& BaseMessage::operator=(BaseMessage&& other){
    id_ = other.id_;
    return *this;
}