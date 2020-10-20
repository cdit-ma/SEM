#ifndef CORE_LOGGERS_EXPERIMENT_LOGGER_H
#define CORE_LOGGERS_EXPERIMENT_LOGGER_H

#include <mutex>
#include <string>

#include "../logger.h"
#include "protowriter.h"
#include <chrono>
#include <exception>

namespace Logan{
// REVIEW (Mitch): What's the difference between an ExperimentLogger and a LoganLogger?
//   This doesn't inherit from Logger?
class ExperimentLogger {
public:
    // REVIEW (Mitch): This constructor is a good candidate for and experimentLogger config struct
    // argument
    ExperimentLogger(const std::string& experiment_name,
                     const std::string& endpoint,
                     ::Logger::Mode mode);
    void LogLifecycleEvent(const ::Logger::LifeCycleEvent& event);

            const std::string& GetExperimentName() const;
        private:
            void PushMessage(std::unique_ptr<google::protobuf::MessageLite> message);

            // REVIEW (Mitch): This mutex is unnecessary, experiment_name_ is only ever read.
            mutable std::mutex mutex_;
            std::unique_ptr<zmq::ProtoWriter> writer_;
            const std::string experiment_name_;
    };
};

#endif //CORE_LOGGERS_EXPERIMENT_LOGGER_H