#include "WE_UTE.h"
#include "WE_UTE_Impl.h"
#include <sstream>
#include <iostream>
#include <stdarg.h>

WE_UTE::WE_UTE(void){
    impl_ = new WE_UTE_Impl();
}

WE_UTE::~WE_UTE(void){
    if (0 != this->impl_){
        delete this->impl_;
        this->impl_ = 0;
    }
}

double WE_UTE::TimeOfDay(){
    return impl_->TimeOfDay();
}

std::string WE_UTE::TimeOfDayString(){
    return impl_->TimeOfDayString();
}

double WE_UTE::EvaluateComplexity(const char* complexity, ...){
    va_list args;
    va_start(args, complexity);
    auto out = impl_->EvaluateComplexity(complexity, args);
    va_end(args);
    return out;
}

void WE_UTE::DebugMessage(const char* fmtstr, ...){
    va_list args;
    va_start(args, fmtstr);
    impl_->DebugMessage(fmtstr, args);
    va_end(args);
}
