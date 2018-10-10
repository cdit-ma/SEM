#include "cpu_worker.h"
#include "cpu_worker_impl.h"
#include <core/component.h>


Cpu_Worker::Cpu_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Cpu_Worker_Impl>(new Cpu_Worker_Impl());
}

int Cpu_Worker::IntOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->IntOp(loop);
    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::FloatOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->FloatOp(loop);
    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::Whetstone(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->Whetstone(loop);
    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::Dhrystone(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->Dhrystone(loop);
    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::MWIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);
    //Log Before
    int result = impl_->MWIP(loop);
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    //Log After
    return result;
}

int Cpu_Worker::DMIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->DMIP(loop);
    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
                        std::vector<float> &matrixC){
    auto work_id = get_new_work_id();
    auto fun = std::string(GET_FUNC);
    auto args = get_arg_string_variadic("matrixA size = %lf; matrixB size = %lf; matrixC size = %lf", 
                                        matrixA.size(), matrixB.size(), matrixC.size());

    //Log Before
    Log(fun, Logger::WorkloadEvent::STARTED, work_id, args);

    int result = impl_->MatrixMult(matrixA.size(), matrixB.size(), matrixC.size(), 
                                   matrixA.data(), matrixB.data(), matrixC.data());
    //Log After
    Log(fun, Logger::WorkloadEvent::FINISHED, work_id);
    return result;
}