#ifndef CORE_LOGGERPRINTER_H
#define CORE_LOGGERPRINTER_H

#include <mutex>
#include <string>

#include <core/logger.h>

class LoggerPrinter : public Logger{
    protected:
    public:
        static Logger& get_logger();

        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, bool print);
        void LogComponentMessage(const Component& component, const std::string& message);

        void LogLifecycleException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event);
        void LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event);
        void LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event);
        void LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string, bool print);
        void LogPortExceptionEvent(const Port& port, const std::string& error_string, bool print);
    private:
        std::mutex mutex_;
};

#endif //CORE_LOGGERPRINTER_H