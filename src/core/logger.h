#ifndef CORE_LOGGER_H
#define CORE_LOGGER_H

#include <string>


#ifdef _WIN32
    #define GET_FUNC __FUNCTION__
#else
    #define GET_FUNC __PRETTY_FUNCTION__
#endif

class Worker;
class Component;
class Port;
class BaseMessage;
class Activatable;

class Logger{
    public:
        enum class Mode {
            OFF,
            LIVE,
            CACHED
        };

        enum class LifeCycleEvent{
            STARTED = 0,
            CONFIGURED = 1,
            ACTIVATED = 2,
            PASSIVATED = 3,
            TERMINATED = 4,
        };

        enum class WorkloadEvent{
            STARTED = 0,
            FINISHED = 1,
            MESSAGE = 2,
            WARNING = 3,
            ERROR = 4
        };

        enum class ComponentEvent{
            SENT = 0,
            RECEIVED = 1,
            STARTED_FUNC = 2,
            FINISHED_FUNC = 3,
            IGNORED = 4,
        };
    
    public:
        virtual void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int log_level, int work_id = -1, std::string args = "") = 0;
        virtual void LogComponentMessage(const Component& component, const std::string& message) = 0;

        virtual void LogLifecycleException(const Activatable& entity, const std::string& message) = 0;
        virtual void LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event) = 0;
        virtual void LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event) = 0;
        virtual void LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event) = 0;
        virtual void LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string) = 0;
        virtual void LogPortExceptionEvent(const Port& port, const std::string& error_string) = 0;
};

#endif //CORE_LOGGER_H