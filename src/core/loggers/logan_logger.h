#ifndef CORE_LOGGERS_LOGAN_LOGGER_H
#define CORE_LOGGERS_LOGAN_LOGGER_H

#include <mutex>
#include <string>

#include <core/logger.h>
#include <zmq/protowriter/cachedprotowriter.h>
#include <chrono>
#include <exception>

// REVIEW (Mitch): See refactor strategy detailed in ../logger.h
namespace Logan{
// REVIEW (Mitch): Move out of Logan namespace
// REVIEW (Mitch): Clarify that this logger implementation logs model events to Logan.
    class Logger : public ::Logger{
        public:

        // REVIEW (Mitch): Major code smell, only used as throw type from logger construction.
        //  Logan::Logger throws NotNeededException when mode passed to constructor is OFF
        //  This check should be done at call site.
            class NotNeededException : public std::runtime_error{
                public:
                    NotNeededException(const std::string& what_arg) : std::runtime_error(what_arg){};
            };

        // REVIEW (Mitch): This is a huge constructor arg list, consider a LoggerConfig struct (builder pattern?)
            Logger(const std::string& experiment_name, const std::string& host_name, const std::string& container_name, const std::string& container_id, const std::string& address, const std::string& port, Logger::Mode mode);
            
            void LogMessage(const Activatable& entity, const std::string& message);
            // REVIEW (Mitch): Consider taking an exception as arg type
            void LogException(const Activatable& entity, const std::string& message);
            void LogLifecycleEvent(const Activatable& entity, const ::Logger::LifeCycleEvent& event);
            // REVIEW (Mitch): Refactor these functions to take Event structs as second param. Event struct captures event type + other params as typed struct
            void LogWorkerEvent(const Worker& worker, const std::string& function_name, const ::Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level);
            void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const ::Logger::UtilizationEvent& event, const std::string& message_str);
            const std::string& GetExperimentName() const;
            const std::string& GetHostName() const;
            const std::string& GetContainerId() const;
            const std::string& GetContainerName() const;
        private:
        // REVIEW (Mitch): Decompose to LogMessage and LogException
        // REVIEW (Mitch): RENAME, ambiguity between this (private LogMessage(3args) and public LogMessage(2args)
            void LogMessage(const Activatable& entity, bool is_exception, const std::string& message);

            void PushMessage(std::unique_ptr<google::protobuf::MessageLite> message);
            // REVIEW (Mitch): Remove this mutex, only used when reading exp_name_, host_name_, container_id_, container_name_.
            //  These vals are never written to after construction...
            mutable std::mutex mutex_;
            std::unique_ptr<zmq::ProtoWriter> writer_;
            // REVIEW (Mitch): Const data members implicitly delete move assignment
            // REVIEW (Mitch): Why does this class even have these data members?
            const std::string experiment_name_;
            const std::string host_name_;
            const std::string container_id_;
            const std::string container_name_;
    };
};

#endif //CORE_LOGGERS_LOGAN_LOGGER_H