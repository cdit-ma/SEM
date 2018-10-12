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

        void LogMessage(const Activatable& entity, const std::string& message);
        void LogException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event);
        
        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id = -1, std::string args = "", int message_log_level = -1);
        void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str = "");
    private:
        void RunOnLoggers(std::function<void (Logger&)> func);

        std::mutex mutex_;
        std::vector<std::reference_wrapper<Logger>> attached_loggers_;
};

#endif //CORE_LOGGERPROXY_H