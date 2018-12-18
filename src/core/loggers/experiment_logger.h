#ifndef CORE_LOGGERS_EXPERIMENT_LOGGER_H
#define CORE_LOGGERS_EXPERIMENT_LOGGER_H

#include <mutex>
#include <string>

#include <core/logger.h>
#include <zmq/protowriter/protowriter.h>
#include <chrono>
#include <exception>

namespace Logan{
    class ExperimentLogger{
        public:
            ExperimentLogger(const std::string& experiment_name, const std::string& endpoint, ::Logger::Mode mode);
            void LogLifecycleEvent(const ::Logger::LifeCycleEvent& event);

            const std::string& GetExperimentName() const;
        private:
            void PushMessage(std::unique_ptr<google::protobuf::MessageLite> message);

            mutable std::mutex mutex_;
            std::unique_ptr<zmq::ProtoWriter> writer_;
            const std::string experiment_name_;
    };
};

#endif //CORE_LOGGERS_EXPERIMENT_LOGGER_H