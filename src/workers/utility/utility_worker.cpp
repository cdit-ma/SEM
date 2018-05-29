#include "utility_worker.h"
#include "utility_worker_impl.h"
#include <sstream>
#include <iostream>
#include <stdarg.h>

Utility_Worker::Utility_Worker(const Component& component, const std::string& inst_name) : Worker(component, GET_FUNC, inst_name){
    impl_ = new Utility_Worker_Impl();
    impl_->SetRandomSeed(static_cast<unsigned int>(GetTimeOfDay()));
}

Utility_Worker::~Utility_Worker(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

double Utility_Worker::GetTimeOfDay(){
    return impl_->TimeOfDay();
}

std::string Utility_Worker::GetTimeOfDayString(){
    return impl_->TimeOfDayString();
}

double Utility_Worker::EvaluateComplexity(const std::string complexity, ...){
    va_list args;
    va_start(args, complexity);
    auto out = impl_->EvaluateComplexity(complexity.c_str(), args);
    va_end(args);
    return out;
}

void Utility_Worker::Log(const std::string str_format, bool print, ...){
    va_list args;
    va_start(args, print);
    std::string message = get_arg_string(str_format, args);
    va_end(args);

    Worker::Log("LogMessage", ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), message);

    if(print){
        const auto& c = get_container();
        std::cout << c.get_name() << "<" << c.get_type() << ">: ";
        std::cout << message << std::endl;
    }
}



void Utility_Worker::USleep(int microseconds){
    auto id = get_new_work_id();
    Worker::Log("USleep", ModelLogger::WorkloadEvent::MESSAGE, id, "Sleeping for: " + std::to_string(microseconds) + " us");
    impl_->USleep(microseconds);
    Worker::Log("USleep", ModelLogger::WorkloadEvent::MESSAGE, id, "Woken");
}
void Utility_Worker::Sleep(int seconds){
    auto id = get_new_work_id();
    Worker::Log("Sleep", ModelLogger::WorkloadEvent::MESSAGE, id, "Sleeping for: " + std::to_string(seconds) + " s");
    impl_->Sleep(seconds);
    Worker::Log("Sleep", ModelLogger::WorkloadEvent::MESSAGE, id, "Woken");
}


 
void Utility_Worker::SetRandomSeed(unsigned int seed){
    Worker::Log("SetRandomSeed", ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), "Set random seed: " + std::to_string(seed));
    return impl_->SetRandomSeed(seed);
}

int Utility_Worker::RandomUniformInt(int lower_bound, int upper_bound){
    return impl_->RandomUniformInt(lower_bound, upper_bound);
}

double Utility_Worker::RandomUniformReal(double lower_bound, double upper_bound){
    return impl_->RandomUniformReal(lower_bound, upper_bound);
}

double Utility_Worker::RandomNormalReal(double mean, double stddev){
    return impl_->RandomNormalReal(mean, stddev);
}

double Utility_Worker::RandomExponentialReal(double lambda){
    return impl_->RandomExponentialReal(lambda);
}

std::string Utility_Worker::GenerateUUID(){
    return impl_->GenerateUUID();
}
