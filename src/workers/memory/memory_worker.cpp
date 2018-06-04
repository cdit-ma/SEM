#include "memory_worker.h"
#include "memory_worker_impl.h"

Memory_Worker::Memory_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = new Memory_Worker_Impl();
}

Memory_Worker::~Memory_Worker(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

void Memory_Worker::Allocate(size_t kilobytes){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("kilobytes = %lf", kilobytes);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    
    //Do work
    auto res = impl_->Allocate(kilobytes);
    if(!res){
        Log(fun, ModelLogger::WorkloadEvent::MESSAGE, work_id, "Allocation Failed");
    }

    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id, args);
    
}

void Memory_Worker::Deallocate(size_t kilobytes){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("kilobytes = %lf", kilobytes);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);

    //Do work
    auto res = impl_->Deallocate(kilobytes);
    if(!res){
        Log(fun, ModelLogger::WorkloadEvent::MESSAGE, work_id, "Deallocation Failed");
    }

    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id, args);
}

size_t Memory_Worker::GetAllocatedCount() const{
    return impl_->GetAllocatedCount();
}
