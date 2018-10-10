#include "loggerproxy.h"
#include "loggerprinter.h"
#include <algorithm>

LoggerProxy::LoggerProxy(): Logger(){
    AddLogger(LoggerPrinter::get_logger());
}

void LoggerProxy::AddLogger(Logger& logger){
    std::lock_guard<std::mutex> lock(mutex_);
    attached_loggers_.emplace_back(logger);
}

void LoggerProxy::LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){
    RunOnLoggers([&](Logger& l){l.LogWorkerEvent(worker, function_name, event, work_id, args, message_log_level);});
}

void LoggerProxy::LogLifecycleException(const Activatable& entity, const std::string& message){
    RunOnLoggers([&](Logger& l){l.LogLifecycleException(entity, message);});
}

void LoggerProxy::LogComponentMessage(const Component& component, const std::string& message){
    RunOnLoggers([&](Logger& l){l.LogComponentMessage(component, message);});
}

void LoggerProxy::LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event){
    RunOnLoggers([&](Logger& l){l.LogLifecycleEvent(component, event);});
}

void LoggerProxy::LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event){
    RunOnLoggers([&](Logger& l){l.LogLifecycleEvent(port, event);});
}

void LoggerProxy::LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event){
    RunOnLoggers([&](Logger& l){l.LogComponentEvent(port, message, event);});
}

void LoggerProxy::LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string){
    RunOnLoggers([&](Logger& l){l.LogPortExceptionEvent(port, message, error_string);});
}

void LoggerProxy::LogPortExceptionEvent(const Port& port, const std::string& error_string){
    RunOnLoggers([&](Logger& l){l.LogPortExceptionEvent(port, error_string);});
}

void LoggerProxy::RunOnLoggers(std::function<void (Logger&)> func){
    std::lock_guard<std::mutex> lock(mutex_);
    std::for_each(attached_loggers_.begin(), attached_loggers_.end(), [func](std::reference_wrapper<Logger> logger){func(logger.get());});
}