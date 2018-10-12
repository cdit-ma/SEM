#include "logger.h"
int Logger::GetWorkloadLogLevel(const WorkloadEvent& event, int message_log_level){
    switch(event){
        case WorkloadEvent::ERROR:
            return 2;
        case WorkloadEvent::WARNING:
            return 3;
        case WorkloadEvent::MESSAGE:{
            if(message_log_level <= 0 || message_log_level > 4){
                return 4;
            }else{
                return message_log_level;
            }
        }
        case WorkloadEvent::STARTED:
        case WorkloadEvent::FINISHED:
            return 5;
    }
    return 5;
}