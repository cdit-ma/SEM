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
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("kilobytes = %lf", kilobytes);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);

    // Bounds check for negative sizes
    if (kilobytes < 0) {
        Log(fun, Logger::WorkloadEvent::WARNING, work_id, 
            "Allocation Failed; attempting to deallocate with a negative number");
    } else {
        try {
            //Do work
            auto res = impl_->Allocate(kilobytes);
            if(!res){
                Log(fun, Logger::WorkloadEvent::ERROR, work_id, "Allocation Failed");
            }
        } catch (const std::exception& ex) {
            Log(fun, Logger::WorkloadEvent::ERROR, work_id,
                "Allocation failed with exception: "+std::string(ex.what()));
        }
    }

    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id, args);
}

void Memory_Worker::Deallocate(int kilobytes){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("kilobytes = %lf", kilobytes);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);

    // Bounds check for negative sizes
    if (kilobytes < 0) {
        Log(fun, Logger::WorkloadEvent::WARNING, work_id,
            "Deallocation Failed; attempting to deallocate with a negative number");
    } else {
        try {
            //Do work
            auto res = impl_->Deallocate(kilobytes);
            if(!res){
                Log(fun, Logger::WorkloadEvent::ERROR, work_id, "Deallocation Failed");
            }
        } catch (const std::exception& ex) {
            Log(fun, Logger::WorkloadEvent::ERROR, work_id,
                "Deallocation failed with exception: "+std::string(ex.what()));
        }
    }

    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id, args);
}

int Memory_Worker::GetAllocatedCount() const{
    return impl_->GetAllocatedCount();
}
