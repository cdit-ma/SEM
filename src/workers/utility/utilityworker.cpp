#include "utilityworker.h"
#include "utilityworker_impl.h"
#include <sstream>
#include <iostream>
#include <stdarg.h>

UtilityWorker::UtilityWorker(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new UtilityWorker_Impl();
}

UtilityWorker::~UtilityWorker(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

double UtilityWorker::TimeOfDay(){
    return impl_->TimeOfDay();
}

std::string UtilityWorker::TimeOfDayString(){
    return impl_->TimeOfDayString();
}

double UtilityWorker::EvaluateComplexity(const std::string complexity, ...){
    va_list args;
    va_start(args, complexity);
    auto out = impl_->EvaluateComplexity(complexity.c_str(), args);
    va_end(args);
    return out;
}

void UtilityWorker::DebugMessage(const std::string str_format, ...){
    va_list args;
    va_start(args, str_format);
    std::cout << get_arg_string(str_format, args) << std::endl;
    va_end(args);
}