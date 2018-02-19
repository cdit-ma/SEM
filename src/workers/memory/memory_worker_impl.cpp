#include "memory_worker_impl.h"
#include <iostream>

#define CUTS_MEMORY_ALLOC_SIZE 1024

Memory_Worker_Impl::~Memory_Worker_Impl(){
    std::lock_guard<std::mutex> guard(lock_);
    auto size = memory_.size();
    Deallocate_(size);
}

bool Memory_Worker_Impl::Allocate(size_t kilobytes) {
    bool result = false;
    if(kilobytes > 0){
        std::lock_guard<std::mutex> guard(lock_);

        result = Allocate_(kilobytes);
    }
    return result;
}

bool Memory_Worker_Impl::Allocate_(size_t kilobytes){
    bool result = false;
    try{
        // although the complexity function is evaluated to a double, only use integer for allocating memory
        while(kilobytes-- > 0){
            char* allocation = 0;
            allocation = new char[CUTS_MEMORY_ALLOC_SIZE];

            if(allocation){
                memory_.push_back(allocation);
            }
        }
        result = true;
    }
    catch (...){
    }
    return result;
}

bool Memory_Worker_Impl::Deallocate(size_t kilobytes) {
    bool result = false;
    if(kilobytes > 0){
        std::lock_guard<std::mutex> guard(lock_);
        result = Deallocate_(kilobytes);
    }
    return result;
}

bool Memory_Worker_Impl::Deallocate_(size_t kilobytes){
    bool result = false;
    try{
        // Make sure we are not trying to deallocate more memory
        // that what is currently allocated.
        if (kilobytes > memory_.size()){
            kilobytes = memory_.size();
        }
        char* memory = 0;

        while(kilobytes-- > 0){
            // get the next allocation on the <memory_> stack
            memory = memory_.front();
            memory_.pop_front();

            // delete the piece of
            delete[] memory;
        }
        result = true;
    }
    catch(...){
    }
    return result;
}

size_t Memory_Worker_Impl::GetAllocatedCount(){
    std::lock_guard<std::mutex> guard(lock_);
    return memory_.size();
}
