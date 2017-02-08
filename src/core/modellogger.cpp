#include "modellogger.h"

#include <iostream>

#include "../re_common/zmqprotowriter/cachedzmqmessagewriter.h"
#include "../re_common/proto/modelevent/modelevent.pb.h"

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

void ModelLogger::shutdown_logger(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_){
        delete singleton_;
        singleton_ = 0;
    }
}

ModelLogger::ModelLogger(bool cached){
    writer_ = new CachedZMQMessageWriter(1);
   // }else{
       // writer_ = new ZMQMessageWriter();
   // }
    writer_->BindPublisherSocket("tcp://192.168.111.187:8000");
}

ModelLogger::~ModelLogger(){
    std::cout << "~ModelLogger" << std::endl;
    //Flushes writer
    delete writer_;
}

int count = 0;
void ModelLogger::LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event){
    std::cout << "ModelLogger::LogLifecycleEvent()" << std::endl;
    ModelEvent* e = new ModelEvent();
    e->set_hostname("Test Hostname");
    e->set_component_name(component->get_name());
    e->set_timestamp(count ++);

    auto c_e  = e->mutable_component_event();
    c_e->set_id("ID");
    c_e->set_type("TYPE");
    c_e->set_action_type((ActionType)(int)event);
    writer_->PushMessage(e);
}

void ModelLogger::LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event){
    //Do Nothing
}