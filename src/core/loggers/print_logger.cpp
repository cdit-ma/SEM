#include "print_logger.h"
#include <iostream>
#include <core/component.h>
#include <core/ports/port.h>
#include <core/worker.h>

Print::Logger& Print::Logger::get_logger(){
    static std::unique_ptr<Print::Logger> logger(new Print::Logger());
    return *logger;
}

void Print::Logger::SetLogLevel(int level){
    std::lock_guard<std::mutex> lock(mutex_);
    log_level_ = level;
}

bool Print::Logger::ShouldPrint(const int& level) const{
    std::lock_guard<std::mutex> lock(mutex_);
    return log_level_ >= level; 
}

std::ostream& PrintComponent(std::ostream& stream, const Component& component){
    stream << component.GetLocalisedName() <<  " [" << component.get_type() << "|" << component.get_id() << "]";
    return stream;
}

std::ostream& PrintPort(std::ostream& stream, const Port& port){
    auto component = port.get_component().lock();
    if(component){
        PrintComponent(stream, *component) << ": ";
    }
    stream << port.get_name() << "[";

    const auto& type = port.get_type();
    const auto& middleware = port.get_middleware();

    if(type.size()){
        stream << type << "|";
    }
    if(middleware.size()){
        stream << middleware << "|";
    }
    stream << port.get_id() << "]";
    return stream;
}


std::ostream& PrintWorker(std::ostream& stream, const Worker& worker){
    stream << worker.get_name() <<  " [" << worker.get_worker_name() << "|" << worker.get_id() << "] ";
    return stream;
}

void Print::Logger::LogMessage(const Activatable& entity, const std::string& message){

}

void Print::Logger::LogException(const Activatable& entity, const std::string& message){

}

void Print::Logger::LogLifecycleEvent(const Activatable& entity, const ::Logger::LifeCycleEvent& event){

}

std::ostream& Print::Logger::GetStream(int level) const{
    return level <= 2 ? std::cerr : std::cout;
}

void Print::Logger::LogWorkerEvent(const Worker& worker, const std::string& function_name, const ::Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){
    auto log_level = GetWorkloadLogLevel(event, message_log_level);
    if(ShouldPrint(log_level)){
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stream = GetStream(log_level);

        try{
            PrintComponent(stream, worker.get_component()) << ": ";
        }catch(const std::exception& e){}
        
        PrintWorker(stream, worker);
        stream << "<" << GetWorkloadName(event) << "> (" << function_name;

        if(work_id != -1){stream << "|" << work_id;}
        stream << "): ";

        if(args.size()){stream << args;}
        stream << std::endl;
    }
}

void Print::Logger::LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const ::Logger::UtilizationEvent& event, const std::string& message_str){
    auto log_level = GetUtilizationLogLevel(event);
    if(ShouldPrint(log_level)){

        std::lock_guard<std::mutex> lock(mutex_);
        auto& stream = GetStream(log_level);

        PrintPort(stream, port);
        stream << "<" << GetUtilizationName(event) << "|" << message.get_base_message_id() << "> ";

        if(message_str.size()){
            stream << message_str;
        }

        stream << std::endl;
    }
}