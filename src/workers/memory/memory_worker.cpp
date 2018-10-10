#include "memory_worker.h"
#include "memory_worker_impl.h"
#include <exception>

const int MEM_WORKER_LOG_LEVEL = 8;

Memory_Worker::Memory_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Memory_Worker_Impl>(new Memory_Worker_Impl());
}

void Memory_Worker::Allocate(int kilobytes){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("kilobytes = %lf", kilobytes);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, MEM_WORKER_LOG_LEVEL, work_id, args);

    // Bounds check for negative sizes
    if (kilobytes < 0) {
        Log(fun, Logger::WorkloadEvent::WARNING, MEM_WORKER_LOG_LEVEL, work_id, 
            "Allocation Failed; attempting to deallocate with a negative number");
    } else {
        try {
            //Do work
            auto res = impl_->Allocate(kilobytes);
            if(!res){
                Log(fun, Logger::WorkloadEvent::ERROR, MEM_WORKER_LOG_LEVEL, work_id, "Allocation Failed");
            }
        } catch (const std::exception& ex) {
            Log(fun, Logger::WorkloadEvent::ERROR, MEM_WORKER_LOG_LEVEL, work_id,
                "Allocation failed with exception: "+std::string(ex.what()));
        }
    }

    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, MEM_WORKER_LOG_LEVEL, work_id, args);
}

void Memory_Worker::Deallocate(int kilobytes){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("kilobytes = %lf", kilobytes);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, MEM_WORKER_LOG_LEVEL, work_id, args);

    // Bounds check for negative sizes
    if (kilobytes < 0) {
        Log(fun, Logger::WorkloadEvent::WARNING, MEM_WORKER_LOG_LEVEL, work_id,
            "Deallocation Failed; attempting to deallocate with a negative number");
    } else {
        try {
            //Do work
            auto res = impl_->Deallocate(kilobytes);
            if(!res){
                Log(fun, Logger::WorkloadEvent::ERROR, MEM_WORKER_LOG_LEVEL, work_id, "Deallocation Failed");
            }
        } catch (const std::exception& ex) {
            Log(fun, Logger::WorkloadEvent::ERROR, MEM_WORKER_LOG_LEVEL, work_id,
                "Deallocation failed with exception: "+std::string(ex.what()));
        }
    }

    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, MEM_WORKER_LOG_LEVEL, work_id, args);
}

int Memory_Worker::GetAllocatedCount() const{
    return impl_->GetAllocatedCount();
}
