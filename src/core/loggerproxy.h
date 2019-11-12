#ifndef CORE_LOGGERPROXY_H
#define CORE_LOGGERPROXY_H

#include <string>
#include <core/logger.h>
#include <functional>
#include <vector>
#include <mutex>

// REVIEW (Mitch): This is a log multiplexer, at least rename to reflect behaviour.

// REVIEW (Mitch): Pending refactor outlined in logger.h (re. logger + log sink design) this class can be removed.
//  Behaviour is emulated by simply allowing multiple sinks per logger

class LoggerProxy : public Logger{
    public:
        LoggerProxy();
        // REVIEW (Mitch): Review ownership patterns used here. Upon redesign/rework potentially have Logger take
        //  ownership of LogSinks
        void AddLogger(Logger& logger);

        void LogMessage(const Activatable& entity, const std::string& message);
        void LogException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event);
        
        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id = -1, std::string args = "", int message_log_level = -1);
        void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str = "");
    private:
        void RunOnLoggers(std::function<void (Logger&)> func);

        std::mutex mutex_;
        // REVIEW (Mitch): This has a very high chance of one day containing invalidated references.
        std::vector<std::reference_wrapper<Logger>> attached_loggers_;
};

#endif //CORE_LOGGERPROXY_H