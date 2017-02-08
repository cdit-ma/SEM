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
int count = 0;

ModelLogger::ModelLogger(bool cached){
    writer_ = new CachedZMQMessageWriter(5);
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

void fill_info(re_common::Info* info){
    if(info){
        info->set_hostname("HOSTNAME");
        info->set_timestamp(count ++);
        info->set_uid(12);
    }
}

void fill_component(re_common::Component* c, Component* component){
    if(c && component){
        c->set_name(component->get_name());
        c->set_id("C_ID");
        c->set_type("C_TYPE");
    }
}

void fill_port(re_common::Port* p, EventPort* eventport){
    if(p && eventport){
        p->set_name(eventport->get_name());
        p->set_id("P_ID");
        p->set_type("P_TYPE");
        p->set_kind((re_common::Port::PortKind)((int)eventport->get_type()));
    }
}
void ModelLogger::LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event){
    std::cout << "ModelLogger::LogLifecycleEvent()::Component" << std::endl;
    auto e = new re_common::LifecycleEvent();
    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    
    e->set_type((re_common::LifecycleEvent::Type)(int)event);
    
    writer_->PushMessage(e);
}

void ModelLogger::LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event){
    //Do Nothing
    auto e = new re_common::LifecycleEvent();
    auto component = eventport->get_component();
    //Set info

    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    fill_port(e->mutable_port(), eventport);
    e->set_type((re_common::LifecycleEvent::Type)(int)event);

    writer_->PushMessage(e);
}