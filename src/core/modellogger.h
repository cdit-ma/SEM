#ifndef CORE_MODELLOGGER_H
#define CORE_MODELLOGGER_H

#include <mutex>
#include "component.h"
#include "eventport.h"

#include "../re_common/zmqprotowriter/zmqmessagewriter.h"

class ModelLogger{
    public:
        enum LifeCycleEvent{
            STARTED = 0,
            ACTIVATED = 1,
            PASSIVATED = 2,
            TERMINATED = 3,
        };
        //Static getter functions
        static void setup_model_logger(std::string host_name, std::string endpoint, bool cached);
        static ModelLogger* get_model_logger();
        static void shutdown_logger();
        
    protected:
        ModelLogger(std::string host_name, std::string endpoint, bool cached);
        ~ModelLogger();
    public:
        void LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event);
        void LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event);
        void LogMessageEvent(EventPort* eventport);
        const  std::string get_hostname();
    private:

        ZMQMessageWriter* writer_;
        std::string host_name_;
        std::string endpoint_;
        
        static ModelLogger* singleton_;
        static std::mutex global_mutex_;
};

#endif //CORE_MODELLOGGER_H