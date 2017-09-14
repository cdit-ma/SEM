

#include "genericbuffer.h"

GenericBuffer::GenericBuffer(int id) : id_(id) {
    
}

bool GenericBuffer::is_valid() {
    return valid_;
}

int GenericBuffer::GetID() const {
    return id_;
}