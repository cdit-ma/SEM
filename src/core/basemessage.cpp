#include "basemessage.h"


BaseMessage::BaseMessage(){
    this->id_ = counter_++;
}

int BaseMessage::get_base_message_id() const{
    return id_;
}