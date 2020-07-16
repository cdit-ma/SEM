#include "logger.h"
#include <unordered_map>
#include <string>
int Logger::GetWorkloadLogLevel(const WorkloadEvent& event, int message_log_level){
    switch(event){
        case WorkloadEvent::Error:
            return 1;
        case WorkloadEvent::WARNING:
            return 2;
        case WorkloadEvent::MARKER:
            return 5;
        case WorkloadEvent::MESSAGE:{
            if(message_log_level < 0){
                //Default is 3
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
            return 6;
        case UtilizationEvent::EXCEPTION:
            return 1;
    }
}

const std::string Logger::GetUtilizationName(const UtilizationEvent& event){
    switch(event){
        case UtilizationEvent::SENT: return "SENT";
        case UtilizationEvent::RECEIVED: return "RECEIVED";
        case UtilizationEvent::STARTED_FUNC: return "STARTED_FUNC";
        case UtilizationEvent::FINISHED_FUNC: return "FINISHED_FUNC";
        case UtilizationEvent::IGNORED: return "IGNORED";
        case UtilizationEvent::EXCEPTION: return "EXCEPTION";
    }
    return std::string();
}

const std::string Logger::GetWorkloadName(const WorkloadEvent& event){
    switch(event){
        case WorkloadEvent::STARTED: return "STARTED";
        case WorkloadEvent::FINISHED: return "FINISHED";
        case WorkloadEvent::MESSAGE: return "MESSAGE";
        case WorkloadEvent::WARNING: return "WARNING";
        case WorkloadEvent::Error: return "ERROR";
        case WorkloadEvent::MARKER: return "MARKER";
    }
    return std::string();
}

const std::string Logger::GetLifecycleName(const LifeCycleEvent& event){
    switch(event){
        case LifeCycleEvent::STARTED: return "STARTED";
        case LifeCycleEvent::CONFIGURED: return "CONFIGURED";
        case LifeCycleEvent::ACTIVATED: return "ACTIVATED";
        case LifeCycleEvent::PASSIVATED: return "PASSIVATED";
        case LifeCycleEvent::TERMINATED: return "TERMINATED";
    }
    return std::string();
}

std::chrono::microseconds Logger::GetCurrentTime(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
}