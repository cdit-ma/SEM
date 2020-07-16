#ifndef CORE_LOGGER_H
#define CORE_LOGGER_H
// REVIEW (Mitch): Move this file and its .cpp to src/core/loggers

#include <string>
#include <chrono>

#ifdef _WIN32
    #define GET_FUNC __FUNCTION__
#else
    #define GET_FUNC __func__
#endif

class Worker;
class Port;
class BaseMessage;
class Activatable;
// REVIEW (Mitch): To improve testability, implement following:
//   Logger pure virtual interface
//   Log Sink pure virtual interface
//  Dependency inject log sink into logger. Supply convenience functions that construct current
//  logger instances Allows us to build a stubbed out log sink that we can assess state of during
//  testing.

// REVIEW (Mitch): Pure virtual interface for logger. Thoughts on moving to a naming convention for
// interface only classes?
//   e.g. in this case `LoggerInterface` given C++ doesn't have language support to strictly mark
//   interfaces.

// REVIEW (Mitch): Move Logger out of global unqualified namespace. This can lead to ugly,
// unexpected ADL rules
class Logger{
    public:
        enum class Mode {
            OFF = 0,
            LIVE = 1,
            CACHED = 2
        };

        // REVIEW (Mitch): Consider renaming to LifeCycleEventType (or similar).
        //   Create LifeCycleEvent+others structs that capture EventType along with all relevant
        //   event info
        enum class LifeCycleEvent{
            STARTED = 0,
            CONFIGURED = 1,
            ACTIVATED = 2,
            PASSIVATED = 3,
            TERMINATED = 4,
        };

        enum class WorkloadEvent{
            STARTED = 0,
            FINISHED = 1,
            MESSAGE = 2,
            WARNING = 3,
            Error = 4,
            MARKER = 5
        };

        enum class UtilizationEvent{
            SENT = 0,
            RECEIVED = 1,
            STARTED_FUNC = 2,
            FINISHED_FUNC = 3,
            IGNORED = 4,
            EXCEPTION = 5
        };
        

    protected:
        static int GetWorkloadLogLevel(const WorkloadEvent& event, int message_log_level = -1);
        static int GetUtilizationLogLevel(const UtilizationEvent& event);

        // REVIEW (Mitch): static const std::string? Should be static std::string Func() const; ??
        //   Implement as compile time lookup map?
        static const std::string GetUtilizationName(const UtilizationEvent& event);
        static const std::string GetWorkloadName(const WorkloadEvent& event);
        static const std::string GetLifecycleName(const LifeCycleEvent& event);
    public:
        virtual void LogMessage(const Activatable& entity, const std::string& message) = 0;
        virtual void LogException(const Activatable& entity, const std::string& message) = 0;
        virtual void LogLifecycleEvent(const Activatable& entity, const Logger::LifeCycleEvent& event) = 0;
        // REVIEW (Mitch): Default arguments on pure virtual functions behave unexpectedly, Herb
        // Sutter says "DON'T DO THAT"
        //   http://www.gotw.ca/gotw/005.htm
        virtual void LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id = -1, std::string args = "", int message_log_level = -1) = 0;
        virtual void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const Logger::UtilizationEvent& event, const std::string& message_str = "") = 0;

        // REVIEW (Mitch): This doesn't belong here, move to util class
        static std::chrono::microseconds GetCurrentTime();
};

// REVIEW (Mitch): Stop using an unordered map for anything that uses these as keys.
//  Also actually write a hash function if we want a hash function....
namespace std {
    template <> struct hash<Logger::WorkloadEvent> {
        size_t operator() (const Logger::WorkloadEvent &t) const { return size_t(t); }
    };
    template <> struct hash<Logger::UtilizationEvent> {
        size_t operator() (const Logger::UtilizationEvent &t) const { return size_t(t); }
    };
}

#endif //CORE_LOGGER_H