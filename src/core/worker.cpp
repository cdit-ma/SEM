#include "worker.h"
#include <stdio.h>
#include <iostream>
#include <stdarg.h>
#include <core/component.h>

Worker::Worker(const BehaviourContainer& container, const std::string& class_name, const std::string& inst_name, const bool is_worker):
    BehaviourContainer(Class::WORKER, inst_name),
    container_(container),
    worker_name_(class_name),
    is_worker_class_(is_worker)
{
};

Worker::~Worker(){
};

bool Worker::is_custom_class() const{
    return !is_worker_class_;
}

bool Worker::is_worker() const{
    return is_worker_class_;
}

std::string Worker::get_worker_name() const{
    return worker_name_;
};

int Worker::get_new_work_id(){
    std::unique_lock<std::mutex>(mutex_);
    return work_id_ ++;
};

const BehaviourContainer& Worker::get_container() const{
    return container_;
};

const Component& Worker::get_component() const{
    try{
        const auto& component = dynamic_cast<const Component&>(get_container());
        return component;
    }catch(const std::bad_cast& ex){
    
    }

    try{
        const auto& worker = dynamic_cast<const Worker&>(get_container());
        return worker.get_component();
    }catch(const std::bad_cast& ex){
    
    }
    throw std::runtime_error("Worker isn't contained in a Component");
}

std::string Worker::get_arg_string(const std::string str_format, va_list args){
    //We need to make a copy of the arg list before we unwind it.
    va_list arg_copy;
    va_copy(arg_copy, args);
    //Include space for NULL character at end of string
    size_t size = vsnprintf(NULL, 0, str_format.c_str(), arg_copy) + 1;
    //Free memory of our copy
    va_end(arg_copy);
    
    //Dynamically allocate memory for buffer to write into
    char * buffer = new char[size];
    //Write into our buffer using our args list
    vsprintf (buffer, str_format.c_str(), args);
    //Construct a string to return
    std::string str(buffer);
    //Free memory we allocated on the heap
    delete[] buffer;
    return str;
};

std::string Worker::get_arg_string_variadic(const std::string str_format, ...){
    va_list args;
    va_start(args, str_format);
    auto str = get_arg_string(str_format, args);
    va_end(args);
    return str;
};


void Worker::Log(const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){
    logger().LogWorkerEvent(*this, function_name, event, work_id, args, message_log_level);
}