#include "vectormessage.h"

 VectorMessage::VectorMessage() : BaseMessage(0){
     //Construct a BaseMessage
 }

void VectorMessage::dataName(const std::string val){
    this->dataName_ = val;
}

void VectorMessage::data(const std::vector<long> val){
    this->data_ = val;
}

std::string VectorMessage::dataName(){
    return dataName_;
}

std::vector<long> VectorMessage::data(){
    return data_;
}

std::vector<long>& VectorMessage::data_ptr(){
    return data_;
}
