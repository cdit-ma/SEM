#include "modellogger.h"
#include "../re_common/zmqprotowriter/cachedzmqmessagewriter.h"
#include "../re_common/proto/modelevent/modelevent.pb.h"

#include <iostream>

ModelLogger* ModelLogger::singleton_ = 0;
std::mutex ModelLogger::global_mutex_;


ModelLogger* ModelLogger::get_model_logger(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new ModelLogger(true);
        std::cout << "Constructed ModelLogger: " << singleton_ << std::endl;
    }
    return singleton_;
}

ModelLogger::ModelLogger(bool cached){
    if(cached){
        writer_ = new CachedZMQMessageWriter();
    }else{
        writer_ = new ZMQMessageWriter();
    }
}
ModelLogger::~ModelLogger(){
    //Flushes writer
    delete writer_;
}
void ModelLogger::LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event){
    //Do nothiong
}
void ModelLogger::LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event){
    //Do Nothing
}