#include "qpidhelper.h"
#include <iostream>
#include <mutex>
qpid::QpidHelper* qpid::QpidHelper::singleton_ = 0;
std::mutex qpid::QpidHelper::global_mutex_;

qpid::QpidHelper* qpid::QpidHelper::get_zmq_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new QpidHelper();
        std::cout << "Constructed qpid Helper: " << singleton_ << std::endl;
    }
    return singleton_;
};