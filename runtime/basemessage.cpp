#include "basemessage.h"


BaseMessage::BaseMessage(int type){
    this->type_ = type;
    this->id_ = counter_++;
}

int BaseMessage::get_base_message_id() const{
    return id_;
}

int BaseMessage::get_base_message_type() const{
    return type_;
}