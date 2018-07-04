#ifndef CORE_MODELLOGGER_H
#define CORE_MODELLOGGER_H

#include <mutex>
#include <string>

#ifdef _WIN32
    #define GET_FUNC __FUNCTION__
#else
    #define GET_FUNC __PRETTY_FUNCTION__
#endif

class Worker;
class Component;
class Port;
class BaseMessage;
class Activatable;

namespace zmq{
    class ProtoWriter;
};
namespace google{namespace protobuf{class MessageLite;}};

class ModelLogger{
    public:
        enum class Mode {OFF, LIVE, CACHED};

        enum class LifeCycleEvent{
            STARTED = 0,
            ACTIVATED = 1,
            PASSIVATED = 2,
            TERMINATED = 3,
        };

        enum class WorkloadEvent{
            STARTED = 0,
            FINISHED = 1,
            MESSAGE = 2,
        };

        enum class ComponentEvent{
            SENT = 0,
            RECEIVED = 1,
            STARTED_FUNC = 2,
            FINISHED_FUNC = 3,
            IGNORED = 4,
        };
        //Static getter functions
        static bool setup_model_logger(const std::string& host_name, const std::string& endpoint, Mode mode);
        static ModelLogger& get_model_logger();
        static bool shutdown_logger();
        
    protected:
        void setup_logger(const std::string& host_name, const std::string& endpoint, Mode mode);
        bool is_setup();
    public:
        void LogWorkerEvent(const Worker& worker, std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");
        void LogMessageEvent(const Port& port);
        void LogUserMessageEvent(const Component& component, std::string message);
        void LogUserFlagEvent(const Component& component, std::string message);

        
        void LogLifecycleEvent(const Component& component, ModelLogger::LifeCycleEvent event);
        void LogLifecycleEvent(const Port& port, ModelLogger::LifeCycleEvent event);
        void LogComponentEvent(const Port& port, const ::BaseMessage& message, ModelLogger::ComponentEvent event);
        void LogFailedComponentConstruction(std::string component_type, std::string component_name, std::string component_id);
        void LogFailedPortConstruction(std::string component_type, std::string component_name, std::string component_id);

        const std::string& get_hostname() const;
        void set_hostname(const std::string& hostname);

    private:
        void PushMessage(google::protobuf::MessageLite* message);
        std::unique_ptr<zmq::ProtoWriter> writer_;
        bool active_ = true;

        std::string host_name_;
        std::mutex mutex_;
};

enum class Severity{
    FATAL = 0,
    ERROR_ = 1,
    WARNING = 2,
    INFO = 3,
    DEBUG = 4
};



class Log{
    public:
        Log& Msg(const std::string& message);
        Log& Context(Activatable* context);
        Log& Func(const std::string& function_name);
        Log& Class(const std::string& class_name);

        ~Log();
        Log(const Log& log) = delete;
        Log(const Severity& severity);
    private:
        
        std::string message_;
        std::string class_name_;
        std::string function_name_;
        Activatable* context_ = 0;
        Severity severity_;
};


#endif //CORE_MODELLOGGER_H