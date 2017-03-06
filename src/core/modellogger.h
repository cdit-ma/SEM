#ifndef CORE_MODELLOGGER_H
#define CORE_MODELLOGGER_H

#include <mutex>
#include "component.h"
#include "eventport.h"
#include "worker.h"

namespace zmq{
    class ProtoWriter;
};
namespace google{namespace protobuf{class MessageLite;}};

class ModelLogger{
    public:
        enum LifeCycleEvent{
            STARTED = 0,
            ACTIVATED = 1,
            PASSIVATED = 2,
            TERMINATED = 3,
        };
        //Static getter functions
        static bool setup_model_logger(std::string host_name, std::string endpoint, bool cached);
        static ModelLogger* get_model_logger();
        static void shutdown_logger();
        
    protected:
        ModelLogger();
        
        bool setup_logger(bool cached, std::string endpoint);
        bool is_setup();
        void set_hostname(std::string host_name);
        
        zmq::ProtoWriter* writer_;
        ~ModelLogger();
    public:
        void LogWorkerEvent(Worker* worker, std::string function_name, ModelLogger::LifeCycleEvent event, int work_id = -1, std::string args = "");

        void LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event);
        void LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event);
        void LogMessageEvent(EventPort* eventport);
        void LogUserMessageEvent(Component* component, std::string message);
        void LogUserFlagEvent(Component* component, std::string message);
        const std::string get_hostname();
    private:
        void PushMessage(google::protobuf::MessageLite* message);

        std::string host_name_;
        
        static ModelLogger* singleton_;
        static std::mutex global_mutex_;
};

#endif //CORE_MODELLOGGER_H