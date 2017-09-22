

#include "genericbuffer.h"

GenericBuffer::GenericBuffer(int id) : id_(id) {
    
}

bool GenericBuffer::is_valid() {
    return valid_;
}

int GenericBuffer::GetID() const {
    return id_;
}

const cl::Buffer& GenericBuffer::GetBackingRef() const {
    return buffer_;
}