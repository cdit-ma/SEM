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
        static ModelLogger* get_model_logger();
        
    protected:
        ModelLogger(bool cached);
        ~ModelLogger();
    public:
        void LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event);
        void LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event);
    private:
        ZMQMessageWriter* writer_;
        
        static ModelLogger* singleton_;
        static std::mutex global_mutex_;
};

#endif //CORE_MODELLOGGER_H