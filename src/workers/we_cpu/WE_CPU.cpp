#include "WE_CPU.h"
#include "WE_CPU_Impl.h"
#include <iostream>
#include <core/component.h>
#include <core/modellogger.h>

WE_CPU::WE_CPU(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new WE_CPU_Impl();
}

WE_CPU::~WE_CPU(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

int WE_CPU::IntOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string("loop = %lf", loop);
    
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Log Before
    int result = impl_->IntOp(loop);
    //LogWorkerEvent(Worker* worker, std::string function_name, ModelLogger::LifeCycleEvent event, int work_id = -1, std::string args = "");
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    //Log After
    return result;
}

int WE_CPU::FloatOp(double loop){
    return impl_->FloatOp(loop);
}

int WE_CPU::Whetstone(double loop){
    return impl_->Whetstone(loop);
}

int WE_CPU::Dhrystone(double loop){
    return impl_->Dhrystone(loop);
}

int WE_CPU::MWIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string("loop = %lf", loop);
    
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Log Before
    int result = impl_->MWIP(loop);
    //LogWorkerEvent(Worker* worker, std::string function_name, ModelLogger::LifeCycleEvent event, int work_id = -1, std::string args = "");
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    //Log After
    return result;
}

int WE_CPU::DMIP(double loop){
    return impl_->DMIP(loop);
}
