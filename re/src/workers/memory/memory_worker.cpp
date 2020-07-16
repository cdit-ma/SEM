#include "memory_worker.h"
#include "memory_worker_impl.h"
#include <exception>

Memory_Worker::Memory_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Memory_Worker_Impl>(new Memory_Worker_Impl());
}

Memory_Worker::~Memory_Worker(){
    impl_.reset();
}

const std::string& Memory_Worker::get_version() const{
    const static std::string WORKER_VERSION{"1.0.0"};
    return WORKER_VERSION;
}

#include <iostream>
void Memory_Worker::Allocate(int kilobytes){
    const auto& func_name = std::string(GET_FUNC);
    
    auto work_id = get_new_work_id();
    auto args = get_arg_string_variadic("kilobytes = %d", kilobytes);

    try{
        Log(func_name, Logger::WorkloadEvent::STARTED, work_id, args);
        if(kilobytes <= 0){
            throw std::invalid_argument("Got invalid (non-positive) KB allocation.");
        }
        impl_->Allocate(kilobytes);
        Log(func_name, Logger::WorkloadEvent::FINISHED, work_id);
    }catch(const std::exception& ex){
        Log(func_name, Logger::WorkloadEvent::Error, work_id, "Allocation failed with exception: " + std::string(ex.what()));
    }
}

void Memory_Worker::Deallocate(int kilobytes){
    const auto& func_name = std::string(GET_FUNC);
    
    auto work_id = get_new_work_id();
    auto args = get_arg_string_variadic("kilobytes = %d", kilobytes);

    try{
        Log(func_name, Logger::WorkloadEvent::STARTED, work_id, args);
        if(kilobytes <= 0){
            throw std::invalid_argument("Got invalid (non-positive) KB deallocation.");
        }
        impl_->Deallocate(kilobytes);
        Log(func_name, Logger::WorkloadEvent::FINISHED, work_id);
    }catch(const std::exception& ex){
        Log(func_name, Logger::WorkloadEvent::Error, work_id, "Deallocation failed with exception: " + std::string(ex.what()));
    }
}

int Memory_Worker::GetAllocatedCount() const{
    return impl_->GetAllocatedCount();
}
