#include "logger.h"
#include <unordered_map>
#include <string>
int Logger::GetWorkloadLogLevel(const WorkloadEvent& event, int message_log_level){
    switch(event){
        case WorkloadEvent::ERROR:
            return 1;
        case WorkloadEvent::WARNING:
            return 2;
        case WorkloadEvent::MESSAGE:{
            if(message_log_level <= 3){
                return 3;
            }else{
                return message_log_level;
            }
        }
        case WorkloadEvent::STARTED:
        case WorkloadEvent::FINISHED:
            return 6;
    }
    return 5;
}

int Logger::GetUtilizationLogLevel(const UtilizationEvent& event){
    switch(event){
        case UtilizationEvent::SENT:
        case UtilizationEvent::RECEIVED:
            return 6;
        case UtilizationEvent::STARTED_FUNC:
        case UtilizationEvent::FINISHED_FUNC:
            return 7;
        case UtilizationEvent::IGNORED:
            return 2;
        case UtilizationEvent::EXCEPTION:
            return 1;
    }
}

const std::string& Logger::GetUtilizationName(const UtilizationEvent& event){
    static std::unordered_map<UtilizationEvent, std::string > lookup_({
        {UtilizationEvent::SENT, "SENT"},
        {UtilizationEvent::RECEIVED, "RECEIVED"},
        {UtilizationEvent::STARTED_FUNC, "STARTED_FUNC"},
        {UtilizationEvent::FINISHED_FUNC, "FINISHED_FUNC"},
        {UtilizationEvent::IGNORED, "IGNORED"},
        {UtilizationEvent::EXCEPTION, "EXCEPTION"}
    });
    return lookup_.at(event);
}

const std::string& Logger::GetWorkloadName(const WorkloadEvent& event){
    static std::unordered_map<WorkloadEvent, std::string > lookup_({
        {WorkloadEvent::STARTED, "STARTED"},
        {WorkloadEvent::FINISHED, "FINISHED"},
        {WorkloadEvent::MESSAGE, "MESSAGE"},
        {WorkloadEvent::WARNING, "WARNING"},
        {WorkloadEvent::ERROR, "ERROR"}
    });
    return lookup_.at(event);
}