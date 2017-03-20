#include "utility_worker.h"
#include "utility_worker_impl.h"
#include <sstream>
#include <iostream>
#include <stdarg.h>

Utility_Worker::Utility_Worker(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new Utility_Worker_Impl();
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
        auto c = get_component();
        if(c){
            std::cout << c->get_name() << "<" << c->get_type() << ">: ";
        }
        std::cout << message << std::endl;
    }
}