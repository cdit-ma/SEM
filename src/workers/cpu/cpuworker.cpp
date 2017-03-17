#include "cpuworker.h"
#include "cpuworker_impl.h"
#include <iostream>
#include <core/component.h>
#include <core/modellogger.h>

CpuWorker::CpuWorker(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new CpuWorker_Impl();
}

CpuWorker::~CpuWorker(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

int CpuWorker::IntOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->IntOp(loop);
    //Log After
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int CpuWorker::FloatOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->FloatOp(loop);
    //Log After
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int CpuWorker::Whetstone(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->Whetstone(loop);
    //Log After
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int CpuWorker::Dhrystone(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->Dhrystone(loop);
    //Log After
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int CpuWorker::MWIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Log Before
    int result = impl_->MWIP(loop);
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    //Log After
    return result;
}

int CpuWorker::DMIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->DMIP(loop);
    //Log After
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int CpuWorker::MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
                        std::vector<float> &matrixC){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("matrixA size = %lf; matrixB size = %lf; matrixC size = %lf", 
                                        matrixA.size(), matrixB.size(), matrixC.size());

    //Log Before
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);

    int result = impl_->MatrixMult(matrixA.size(), matrixB.size(), matrixC.size(), 
                                   matrixA.data(), matrixB.data(), matrixC.data());
    //Log After
    logger()->LogWorkerEvent(this, fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}