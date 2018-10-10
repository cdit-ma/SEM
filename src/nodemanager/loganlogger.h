#ifndef LOGANPRINTER_H
#define LOGANPRINTER_H

#include <mutex>
#include <string>

#include <core/logger.h>
#include <zmq/protowriter/cachedprotowriter.h>
#include <chrono>

class LoganLogger : public Logger{
    public:
        LoganLogger(const std::string& experiment_name, const std::string& host_name, const std::string& address, const std::string& port, Logger::Mode mode);
        
        void LogMessage(const Activatable& entity, const std::string& message);
        void LogException(const Activatable& entity, const std::string& message);
        void LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event);
        
        void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level);
        void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str);
        
        static std::chrono::milliseconds GetCurrentTime();
        const std::string& GetExperimentName() const;
        const std::string& GetHostName() const;
    private:
        void LogMessage(const Activatable& entity, bool is_exception, const std::string& message);

        void PushMessage(std::unique_ptr<google::protobuf::MessageLite> message);
        mutable std::mutex mutex_;
        std::unique_ptr<zmq::ProtoWriter> writer_;
        const std::string experiment_name_;
        const std::string host_name_;
};

#endif //LOGANPRINTER_H