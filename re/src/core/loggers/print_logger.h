
#ifndef CORE_LOGGERS_PRINT_LOGGER_H
#define CORE_LOGGERS_PRINT_LOGGER_H

#include <mutex>
#include <string>

#include "logger.h"
// REVIEW (Mitch): See refactor strategy detailed in ../logger.h
namespace Print{
// REVIEW (Mitch): Move Print::Logger out of Print namespace. This seems like a bad way to name
// inherited
//  classes

// REVIEW (Mitch): Investigate using fmtlib (https://github.com/fmtlib/fmt) to format log output
// (implementation detail)
class Logger : public ::Logger {
public:
            static Print::Logger& get_logger();
            void SetLogLevel(int level);

            void LogMessage(const Activatable& entity, const std::string& message);
            void LogException(const Activatable& entity, const std::string& message);
            void LogLifecycleEvent(const Activatable& entity, const ::Logger::LifeCycleEvent& event);
            
            void LogWorkerEvent(const Worker& worker, const std::string& function_name, const ::Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level);
            void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const ::Logger::UtilizationEvent& event, const std::string& message_str);
        private:
            bool ShouldPrint(const int& level) const;
            std::ostream& GetStream(int level) const;
            mutable std::mutex mutex_;

            // REVIEW (Mitch): Change this to an atomic so we don't have to hit a mutex every time
            // we go to log.
            //  Or remove ability to mutate logging level during execution
            int log_level_ = 10;
    };
};

#endif //CORE_LOGGERS_PRINT_LOGGER_H