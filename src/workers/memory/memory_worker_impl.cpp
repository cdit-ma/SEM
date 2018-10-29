#include "memory_worker_impl.h"
#include <iostream>
#include <stdint.h>

const size_t MEMORY_CHUNK_SIZE(1024);

Memory_Worker_Impl::~Memory_Worker_Impl(){
    try{
        Deallocate(SIZE_MAX);
    }catch(const std::exception& ex){
        //Ignore
    }
}

//TODO: handle the case where caller may be interested in allocation of continuous block
void Memory_Worker_Impl::Allocate(const size_t kilobytes) {
    std::lock_guard<std::mutex> guard(lock_);
    
    for(size_t i = 0; i < kilobytes; ++i){
        auto allocation = new char[MEMORY_CHUNK_SIZE];
        if(!allocation){
            throw std::bad_alloc();
        }
        memory_.emplace_back(allocation);
    }
}

void Memory_Worker_Impl::Deallocate(const size_t kilobytes){
    std::lock_guard<std::mutex> guard(lock_);
    bool invalid_dealloc = false;
    size_t dealloc_count = kilobytes;
    // Make sure we are not trying to deallocate more memory than what is currently allocated.
    if(dealloc_count > memory_.size()){
        dealloc_count = memory_.size();
        invalid_dealloc = true;
    }
    
    for(size_t i = 0; i < dealloc_count; i++){
        // get the next allocation on the <memory_> stack
        delete[] memory_.front();
        memory_.pop_front();
    }

    if(invalid_dealloc){
        throw std::invalid_argument("Trying to free : " + std::to_string(kilobytes) + "KB of memory when only: " + std::to_string(dealloc_count) + "KB has been allocated.");
    }
}

size_t Memory_Worker_Impl::GetAllocatedCount(){
    std::lock_guard<std::mutex> guard(lock_);
    return memory_.size();
}
