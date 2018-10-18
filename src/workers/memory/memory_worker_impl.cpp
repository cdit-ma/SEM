#include "memory_worker_impl.h"
#include <iostream>

#define CUTS_MEMORY_ALLOC_SIZE 1024

Memory_Worker_Impl::~Memory_Worker_Impl(){
    std::lock_guard<std::mutex> guard(lock_);
    auto size = memory_.size();
    _NonThreadSafeDeallocate(size);
}

//TODO: handle the case where caller may be interested in allocation of continuous block
bool Memory_Worker_Impl::Allocate(size_t kilobytes) {
    bool result = false;
    std::lock_guard<std::mutex> guard(lock_);
    if(kilobytes > 0){
        // although the complexity function is evaluated to a double, only use integer for allocating memory
        while(kilobytes-- > 0){
            char* allocation = 0;
            allocation = new char[CUTS_MEMORY_ALLOC_SIZE];

            if(allocation){
                memory_.push_back(allocation);
            }
            else {
                result = false;
            }
        }
    }
    return result;
}

bool Memory_Worker_Impl::Deallocate(size_t kilobytes) {
    if(kilobytes > 0){
        std::lock_guard<std::mutex> guard(lock_);
        return _NonThreadSafeDeallocate(kilobytes);
    }
    return false;
}

bool Memory_Worker_Impl::_NonThreadSafeDeallocate(size_t kilobytes) {
    bool result = true;

    // Make sure we are not trying to deallocate more memory
    // that what is currently allocated.
    if (kilobytes > memory_.size()){
        kilobytes = memory_.size();
        //TODO: Report attempt to deallocate more memory than has been allocated
        result = false;
    }
    char* memory = 0;

    while(kilobytes-- > 0){
        // get the next allocation on the <memory_> stack
        memory = memory_.front();
        memory_.pop_front();

        // delete the piece of memory
        delete[] memory;
    }

    return result;
}

size_t Memory_Worker_Impl::GetAllocatedCount(){
    std::lock_guard<std::mutex> guard(lock_);
    return memory_.size();
}
