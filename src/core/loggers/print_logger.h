
#ifndef CORE_LOGGERS_PRINT_LOGGER_H
#define CORE_LOGGERS_PRINT_LOGGER_H

#include <mutex>
#include <string>

#include <core/logger.h>

namespace Print{
    class Logger : public ::Logger{
        protected:
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
            int log_level_ = 10;
    };
};

#endif //CORE_LOGGERS_PRINT_LOGGER_H