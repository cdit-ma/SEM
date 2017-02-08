#include "modellogger.h"

#include <ctime>
#include <iostream>

#include "../re_common/zmqprotowriter/cachedzmqmessagewriter.h"
#include "../re_common/proto/modelevent/modelevent.pb.h"

ModelLogger* ModelLogger::singleton_ = 0;
std::mutex ModelLogger::global_mutex_;


void ModelLogger::setup_model_logger(std::string host_name, std::string endpoint, bool cached){
    std::lock_guard<std::mutex> lock(global_mutex_);
    
    if(singleton_ == 0){
        singleton_ = new ModelLogger(host_name, endpoint, cached);
        std::cerr << "Model Logger Constructed!" << std::endl;
    }
}

ModelLogger* ModelLogger::get_model_logger(){
    std::lock_guard<std::mutex> lock(global_mutex_);
    
    if(singleton_ == 0){
        std::cerr << "Model Logger hasn't been setup!" << std::endl;
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

ModelLogger::ModelLogger(std::string host_name, std::string endpoint, bool cached){
    this->host_name_ = host_name;
    this->endpoint_ = endpoint;
    
    if(cached){
        writer_ = new CachedZMQMessageWriter();
    }else{
        writer_ = new ZMQMessageWriter();
    }
    
    writer_->BindPublisherSocket(endpoint_);
}

ModelLogger::~ModelLogger(){
    std::cout << "~ModelLogger" << std::endl;
    //Flushes writer
    delete writer_;
}
const std::string ModelLogger::get_hostname(){
    return host_name_;
}


std::chrono::milliseconds get_current_time(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}


void fill_info(re_common::Info* info){
    if(info){
        info->set_hostname(ModelLogger::get_model_logger()->get_hostname());
        info->set_timestamp(get_current_time().count() / 1000.0);
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

void ModelLogger::LogMessageEvent(EventPort* eventport){
    //Do Nothing
    auto e = new re_common::MessageEvent();
    auto component = eventport->get_component();
    //Set info

    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    fill_port(e->mutable_port(), eventport);
    writer_->PushMessage(e);
}