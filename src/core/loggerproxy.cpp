#include "loggerproxy.h"
#include <algorithm>

LoggerProxy::LoggerProxy(): Logger(){

}

void LoggerProxy::AddLogger(Logger& logger){
    std::lock_guard<std::mutex> lock(mutex_);
    attached_loggers_.emplace_back(logger);
}

void LoggerProxy::LogMessage(const Activatable& entity, const std::string& message){
    RunOnLoggers([&](Logger& l){l.LogMessage(entity, message);});
}

void LoggerProxy::LogException(const Activatable& entity, const std::string& message){
    RunOnLoggers([&](Logger& l){l.LogException(entity, message);});
}

void LoggerProxy::LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event){
    RunOnLoggers([&](Logger& l){l.LogLifecycleEvent(entity, event);});
}

void LoggerProxy::LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){
    RunOnLoggers([&](Logger& l){l.LogWorkerEvent(worker, function_name, event, work_id, args, message_log_level);});
}

void LoggerProxy::LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str){
    RunOnLoggers([&](Logger& l){l.LogPortUtilizationEvent(port, message, event, message_str);});
}

void LoggerProxy::RunOnLoggers(std::function<void (Logger&)> func){
    std::lock_guard<std::mutex> lock(mutex_);
    std::for_each(attached_loggers_.begin(), attached_loggers_.end(), [func](std::reference_wrapper<Logger> logger){func(logger.get());});
}