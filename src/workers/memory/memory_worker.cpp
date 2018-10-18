#include "memory_worker.h"
#include "memory_worker_impl.h"
#include <exception>

Memory_Worker::Memory_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Memory_Worker_Impl>(new Memory_Worker_Impl());
}
Memory_Worker::~Memory_Worker(){
    impl_.reset();
}

void Memory_Worker::Allocate(int kilobytes){
    const auto& func_name = std::string(GET_FUNC);
    
    auto work_id = get_new_work_id();
    auto args = get_arg_string_variadic("kilobytes = %d", kilobytes);

    if(kilobytes > 0){
        try{
            Log(func_name, Logger::WorkloadEvent::STARTED, work_id, args);
            
            if(impl_->Allocate(kilobytes)){
                Log(func_name, Logger::WorkloadEvent::FINISHED, work_id);
            }else{
                Log(func_name, Logger::WorkloadEvent::ERROR, work_id, "Allocation Failed");
            }
        }catch(const std::exception& ex){
            Log(func_name, Logger::WorkloadEvent::ERROR, work_id, "Allocation failed with exception: " + std::string(ex.what()));
        }
    }else{
        Log(func_name, Logger::WorkloadEvent::WARNING, work_id, "Allocation Failed; Attempting to allocate a non-positive number of KB.");
    }
}

void Memory_Worker::Deallocate(int kilobytes){
    const auto& func_name = std::string(GET_FUNC);
    
    auto work_id = get_new_work_id();
    auto args = get_arg_string_variadic("kilobytes = %d", kilobytes);

    if(kilobytes > 0){
        try{
            Log(func_name, Logger::WorkloadEvent::STARTED, work_id, args);
            
            if(impl_->Deallocate(kilobytes)){
                Log(func_name, Logger::WorkloadEvent::FINISHED, work_id);
            }else{
                Log(func_name, Logger::WorkloadEvent::ERROR, work_id, "Deallocation Failed");
            }
        }catch(const std::exception& ex){
            Log(func_name, Logger::WorkloadEvent::ERROR, work_id, "Deallocation failed with exception: " + std::string(ex.what()));
        }
    }else{
        Log(func_name, Logger::WorkloadEvent::WARNING, work_id, "Deallocation Failed; Attempting to deallocate a non-positive number of KB.");
    }
}

int Memory_Worker::GetAllocatedCount() const{
    return impl_->GetAllocatedCount();
}
