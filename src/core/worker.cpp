#include "worker.h"
#include <stdio.h>
#include <iostream>
#include <stdarg.h>

Worker::Worker(Component* component, std::string worker_name, std::string inst_name){
    set_name(inst_name);
    component_ = component;
    worker_name_ = worker_name;
};

Worker::~Worker(){

};

std::string Worker::get_worker_name(){
    return worker_name_;
};

int Worker::get_new_work_id(){
    std::unique_lock<std::mutex>(mutex_);
    return work_id_ ++;
};

Component* Worker::get_component(){
    return component_;
};

std::string Worker::get_arg_string(const std::string str_format, va_list args){
    auto fmt = str_format.c_str();
    int len = snprintf(NULL, 0, fmt, args) + 1;
    char buffer[len];
    vsprintf(buffer, fmt, args);
    return std::string(buffer);
};
