#ifndef CORE_LOGGER_H
#define CORE_LOGGER_H

#include <string>
#include <chrono>

#ifdef _WIN32
    #define GET_FUNC __FUNCTION__
#else
    #define GET_FUNC __func__
#endif

class Worker;
class Port;
class BaseMessage;
class Activatable;

class Logger{
    public:
        enum class Mode {
            OFF = 0,
            LIVE = 1,
            CACHED = 2
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

        enum class UtilizationEvent{
            SENT = 0,
            RECEIVED = 1,
            STARTED_FUNC = 2,
            FINISHED_FUNC = 3,
            IGNORED = 4,
            EXCEPTION = 5
        };
        

    protected:
        static int GetWorkloadLogLevel(const WorkloadEvent& event, int message_log_level = -1);
        static int GetUtilizationLogLevel(const UtilizationEvent& event);
        
        static const std::string GetUtilizationName(const UtilizationEvent& event);
        static const std::string GetWorkloadName(const WorkloadEvent& event);
        static const std::string GetLifecycleName(const LifeCycleEvent& event);
    public:
        virtual void LogMessage(const Activatable& entity, const std::string& message) = 0;
        virtual void LogException(const Activatable& entity, const std::string& message) = 0;
        virtual void LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event) = 0;
        virtual void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id = -1, std::string args = "", int message_log_level = -1) = 0;
        virtual void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str = "") = 0;
        
        static std::chrono::milliseconds GetCurrentTime();
};

namespace std {
    template <> struct hash<Logger::WorkloadEvent> {
        size_t operator() (const Logger::WorkloadEvent &t) const { return size_t(t); }
    };
    template <> struct hash<Logger::UtilizationEvent> {
        size_t operator() (const Logger::UtilizationEvent &t) const { return size_t(t); }
    };
}

#endif //CORE_LOGGER_H