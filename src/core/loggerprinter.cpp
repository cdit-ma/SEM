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
    std::lock_guard<std::mutex> lock(mutex_);
    log_level_ = level;
}

bool LoggerPrinter::Print(const int& level) const{
    std::lock_guard<std::mutex> lock(mutex_);
    return log_level_ >= level; 
}

void LoggerPrinter::LogMessage(const Activatable& entity, const std::string& message){}
void LoggerPrinter::LogException(const Activatable& entity, const std::string& message){}
void LoggerPrinter::LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event){}

void LoggerPrinter::LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){}
void LoggerPrinter::LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str){}