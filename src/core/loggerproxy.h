#ifndef CORE_LOGGERPROXY_H
#define CORE_LOGGERPROXY_H

#include <string>
#include <core/logger.h>
#include <functional>
#include <vector>
#include <mutex>

class LoggerProxy : public Logger{
    public:
        LoggerProxy();
        void AddLogger(Logger& logger);

        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id = -1, std::string args = "", int message_log_level = -1);
        void LogComponentMessage(const Component& component, const std::string& message);

        void LogLifecycleException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event);

        void LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event);
        void LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event);
        void LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string);
        void LogPortExceptionEvent(const Port& port, const std::string& error_string);
    private:
        void RunOnLoggers(std::function<void (Logger&)> func);

        std::mutex mutex_;
        std::vector<std::reference_wrapper<Logger>> attached_loggers_;
};

#endif //CORE_LOGGERPROXY_H