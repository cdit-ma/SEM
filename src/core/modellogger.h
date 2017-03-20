#ifndef CORE_MODELLOGGER_H
#define CORE_MODELLOGGER_H

#include <mutex>
#include "component.h"
#include "eventport.h"

class Worker;

namespace zmq{
    class ProtoWriter;
};
namespace google{namespace protobuf{class MessageLite;}};

class ModelLogger{
    public:
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
        static bool setup_model_logger(std::string host_name, std::string endpoint, bool cached, bool active = true);
        static ModelLogger* get_model_logger();
        static bool shutdown_logger();
        
    protected:
        ModelLogger();
        
        bool setup_logger(bool cached, std::string endpoint, bool active = true);
        bool is_setup();
        void set_hostname(std::string host_name);
        
        zmq::ProtoWriter* writer_;
        ~ModelLogger();
    public:
        void LogWorkerEvent(Worker* worker, std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");

        void LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event);
        void LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event);

        void LogComponentEvent(EventPort* eventport, ::BaseMessage* message, ModelLogger::ComponentEvent event);

        void LogMessageEvent(EventPort* eventport);
        void LogUserMessageEvent(Component* component, std::string message);
        void LogUserFlagEvent(Component* component, std::string message);
        const std::string get_hostname();
    private:
        void PushMessage(google::protobuf::MessageLite* message);

        bool active_;

        std::string host_name_;
        
        static ModelLogger* singleton_;
        static std::mutex global_mutex_;
};

#endif //CORE_MODELLOGGER_H