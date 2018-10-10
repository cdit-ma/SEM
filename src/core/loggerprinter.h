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

        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int log_level, int work_id, std::string args);
        void LogComponentMessage(const Component& component, const std::string& message);

        void LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event);
        void LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event);
        void LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event);
        
        void LogLifecycleException(const Activatable& entity, const std::string& message);
        void LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string);
        void LogPortExceptionEvent(const Port& port, const std::string& error_string);
    private:
        bool Print(const int& level);
        std::mutex mutex_;
        int log_level_ = 10;
};

#endif //CORE_LOGGERPRINTER_H