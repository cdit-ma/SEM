#include "cpu_worker.h"
#include "cpu_worker_impl.h"
#include "component.h"
#include <list>
#include <functional>
#include <future>



Cpu_Worker::Cpu_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Cpu_Worker_Impl>(new Cpu_Worker_Impl());
}

const std::string& Cpu_Worker::get_version() const{
    const static std::string WORKER_VERSION{"1.1.0"};
    return WORKER_VERSION;
}

Cpu_Worker::~Cpu_Worker(){
    impl_.reset();
}

int Cpu_Worker::RunParallel(std::function<int (double)> fn, std::string name, double loop, int thread_count, bool split_workload){
    
    double work_count = loop;
    if(split_workload && thread_count > 0){
        work_count /= thread_count;
    }
    
    auto args = get_arg_string_variadic("threads = %d, loops_per_thread = %lf", thread_count, work_count);
    
    auto work_id = get_new_work_id();
    Log(name, Logger::WorkloadEvent::STARTED, work_id, args);

    std::list< std::future<int> > futures;
    for(int i = 0; i < thread_count; i++){
        futures.push_back(
            std::async(std::launch::async, [work_count, fn]()->int{
                return fn(work_count);
            })
        );
    }

    int error_count = 0;
    std::for_each(futures.begin(), futures.end(), [&error_count](std::future<int> &f){error_count += f.get();});
    Log(name, Logger::WorkloadEvent::FINISHED, work_id);
    return error_count;
}

int Cpu_Worker::IntOp(double loop, int thread_count, bool split_workload){
    return RunParallel([=](double l)->int{return impl_->IntOp(l);}, GET_FUNC, loop, thread_count, split_workload);
}

int Cpu_Worker::FloatOp(double loop, int thread_count, bool split_workload){
    return RunParallel([=](double l)->int{return impl_->FloatOp(l);}, GET_FUNC, loop, thread_count, split_workload);
}

int Cpu_Worker::Whetstone(double loop, int thread_count, bool split_workload){
    return RunParallel([=](double l)->int{return impl_->Whetstone(l);}, GET_FUNC, loop, thread_count, split_workload);
}

int Cpu_Worker::Dhrystone(double loop, int thread_count, bool split_workload){
    return RunParallel([=](double l)->int{return impl_->Dhrystone(l);}, GET_FUNC, loop, thread_count, split_workload);
}

int Cpu_Worker::MWIP(double loop, int thread_count, bool split_workload){
    return RunParallel([=](double l)->int{return impl_->MWIP(l);}, GET_FUNC, loop, thread_count, split_workload);
}

int Cpu_Worker::DMIP(double loop, int thread_count, bool split_workload){
    return RunParallel([=](double l)->int{return impl_->DMIP(l);}, GET_FUNC, loop, thread_count, split_workload);
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