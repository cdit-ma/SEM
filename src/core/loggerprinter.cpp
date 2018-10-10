#include "loggerprinter.h"
#include <iostream>
#include <core/component.h>
#include <core/ports/port.h>
#include <core/worker.h>

Logger& LoggerPrinter::get_logger(){
    static std::unique_ptr<LoggerPrinter> logger(new LoggerPrinter());
    return *logger;
}

void LoggerPrinter::SetLogLevel(int level){
    log_level_ = level;
}

bool LoggerPrinter::Print(const int& level){
    return log_level_ >= level; 
}

void LoggerPrinter::LogLifecycleException(const Activatable& entity, const std::string& message){
    std::lock_guard<std::mutex> lock(mutex_);
    std::cerr << "* Lifecycle Exception: " << entity.get_name() << " [" << entity.get_id() << "] " <<  " | " << message << std::endl;
}
    
void LoggerPrinter::LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int log_level, int work_id, std::string args){
    if(Print(log_level)){
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "* Worker: " << worker.get_name() << " [" << worker.get_type() << " - " << worker.get_id() << "] " << function_name << " | " << args << std::endl;
    }
}

void LoggerPrinter::LogComponentMessage(const Component& component, const std::string& message){
    if(Print(7)){
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "* Component: " << component.get_name() << " [" << component.get_type() << " - " << component.get_id() << "] " << message << std::endl;
    }
}

void LoggerPrinter::LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event){
    if(Print(7)){
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "* Component Lifecycle Event: " << component.get_name() << " [" << component.get_type() << " - " << component.get_id() << "] " << (uint)event << std::endl;
    }
}

void LoggerPrinter::LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event){
    if(Print(7)){
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "* Port Lifecycle Event: " << port.get_name() << " [" << port.get_type() << " - " << port.get_id() << "] " << (uint)event << std::endl;
    }
}

void LoggerPrinter::LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event){
    if(Print(8)){
        auto c_ptr = port.get_component().lock();
        if(c_ptr){
            auto& component = *c_ptr;
            std::lock_guard<std::mutex> lock(mutex_);
            std::cerr << "* Component Event: " << component.get_name() << " [" << component.get_type() << " - " << component.get_id() << "] " << std::endl;
        }
    }
}

void LoggerPrinter::LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string){
    if(Print(1)){
        auto c_ptr = port.get_component().lock();
        if(c_ptr){
            auto& component = *c_ptr;
            std::lock_guard<std::mutex> lock(mutex_);
            std::cerr << "* Port Exception: " << component.get_name() << " [" << component.get_type() << " - " << component.get_id() << "] " << std::endl;
        }
    }
}

void LoggerPrinter::LogPortExceptionEvent(const Port& port, const std::string& error_string){
    if(Print(1)){
        auto c_ptr = port.get_component().lock();
        if(c_ptr){
            auto& component = *c_ptr;
            std::lock_guard<std::mutex> lock(mutex_);
            std::cerr << "* Port Exception: " << component.get_name() << " [" << component.get_type() << " - " << component.get_id() << "] " << std::endl;
        }
    }
}