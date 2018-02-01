#include "environmentrequester.h"

EnvironmentRequester::EnvironmentRequester(){
}

long EnvironmentRequester::Tick(){
    std::unique_lock<std::mutex>(clock_mutex_);
    clock_++;
    return clock_;
}

long EnvironmentRequester::SetClock(long incoming_clock){
    std::unique_lock<std::mutex>(clock_mutex_);
    clock_ = std::max(incoming_clock, clock_) + 1;
    return clock_;
}

long EnvironmentRequester::GetClock(){
    return clock_;
}

