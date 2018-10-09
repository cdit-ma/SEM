#include "loggerproxy.h"
#include "loggerprinter.h"
#include <iostream>

LoggerProxy::LoggerProxy(): Logger(),
    logger_(LoggerEmpty::get_logger())
{
}

Logger& LoggerProxy::GetLogger(){
    return logger_;
}

void LoggerProxy::SetLogger(Logger& logger){
    logger_ = std::ref(logger);
}

void LoggerProxy::LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, bool print){
    GetLogger().LogWorkerEvent(worker, function_name, event, work_id, args, print);
}

void LoggerProxy::LogLifecycleException(const Activatable& entity, const std::string& message){
    GetLogger().LogLifecycleException(entity, message);
}

void LoggerProxy::LogComponentMessage(const Component& component, const std::string& message){
    GetLogger().LogComponentMessage(component, message);
}

void LoggerProxy::LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event){
    GetLogger().LogLifecycleEvent(component, event);
}

void LoggerProxy::LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event){
    GetLogger().LogLifecycleEvent(port, event);
}

void LoggerProxy::LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event){
    GetLogger().LogComponentEvent(port, message, event);
}

void LoggerProxy::LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string, bool print){
    GetLogger().LogPortExceptionEvent(port, message, error_string, print);
}

void LoggerProxy::LogPortExceptionEvent(const Port& port, const std::string& error_string, bool print){
    GetLogger().LogPortExceptionEvent(port, error_string, print);
}