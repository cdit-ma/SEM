#ifndef LOGANPRINTER_H
#define LOGANPRINTER_H

#include <mutex>
#include <string>

#include <core/logger.h>
#include <zmq/protowriter/cachedprotowriter.h>

class LoganLogger : public Logger{
    public:
        LoganLogger(const std::string& experiment_name, const std::string& host_name, const std::string& address, const std::string& port, Logger::Mode mode);
        
        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int log_level, int work_id, std::string args);
        void LogComponentMessage(const Component& component, const std::string& message);

        void LogLifecycleException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event);
        void LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event);
        void LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event);
        void LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string);
        void LogPortExceptionEvent(const Port& port, const std::string& error_string);
    private:
        std::mutex mutex_;
        std::unique_ptr<zmq::ProtoWriter> writer_;
        const std::string experiment_name_;
        const std::string host_name_;
};

#endif //LOGANPRINTER_H