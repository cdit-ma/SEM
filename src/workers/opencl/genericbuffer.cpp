

#include "genericbuffer.h"
#include "OpenCLManager.h"

/*GenericBuffer::GenericBuffer(int id) : id_(id) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(manager)
}*/

GenericBuffer::GenericBuffer(OpenCLManager* manager, size_t size) :
    manager_(manager), size_(size) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(*manager);
}


bool GenericBuffer::is_valid() const{
    return valid_;
}

size_t GenericBuffer::GetSize() const {
    return size_;
}

int GenericBuffer::GetID() const {
    return id_;
}


const cl::Buffer& GenericBuffer::GetBackingRef() const {
    return buffer_;
}