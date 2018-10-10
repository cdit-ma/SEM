#ifndef CORE_LOGGERPRINTER_H
#define CORE_LOGGERPRINTER_H

#include <mutex>
#include <string>

#include <core/logger.h>

class LoggerPrinter : public Logger{
    protected:
    public:
        static Logger& get_logger();
        void SetLogLevel(int level);

        void LogMessage(const Activatable& entity, const std::string& message);
        void LogException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event);
        
        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level);
        void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str);
    private:
        bool Print(const int& level) const;
        mutable std::mutex mutex_;
        int log_level_ = 10;
};

#endif //CORE_LOGGERPRINTER_H