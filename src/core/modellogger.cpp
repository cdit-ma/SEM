#include "modellogger.h"
#include <ctime>
#include <iostream>

#include "../re_common/zmq/protowriter/cachedprotowriter.h"
#include "../re_common/proto/modelevent/modelevent.pb.h"

#include "worker.h"

ModelLogger* ModelLogger::singleton_ = 0;
std::mutex ModelLogger::global_mutex_;


bool ModelLogger::setup_model_logger(std::string host_name, std::string endpoint, Mode mode){
    auto s = get_model_logger();
    bool success = false;
    {
        std::lock_guard<std::mutex> lock(global_mutex_);
        if(!s->is_setup()){
            success = s->setup_logger(endpoint, mode);
            s->set_hostname(host_name);
        }
    }
    return success;
}

ModelLogger* ModelLogger::get_model_logger(){
    std::lock_guard<std::mutex> lock(global_mutex_);
    
    if(singleton_ == 0){
        singleton_ = new ModelLogger();
    }
    return singleton_;
}

bool ModelLogger::shutdown_logger(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_){
        delete singleton_;
        singleton_ = 0;
        return true;
    }
    return false;
}
int count = 0;

ModelLogger::ModelLogger(){
    writer_ = 0;
}

bool ModelLogger::is_setup(){
    return writer_;
}

void ModelLogger::set_hostname(std::string host_name){
    this->host_name_ = host_name;
}

bool ModelLogger::setup_logger(std::string endpoint, Mode mode){
    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%LOGGER%"  + std::to_string((int)mode) + "%" << std::endl;
    if(mode == Mode::OFF){
        active_ = false;
    }
    if(!writer_){
        if(mode == Mode::LIVE){
            writer_ = new zmq::ProtoWriter();
        }else{
            writer_ = new zmq::CachedProtoWriter();
        }
        return writer_->BindPublisherSocket(endpoint);
    }
    return false;
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
        c->set_id(component->get_id());
        c->set_type(component->get_type());
    }
}

void fill_port(re_common::Port* p, EventPort* eventport){
    if(p && eventport){
        p->set_name(eventport->get_name());
        p->set_id(eventport->get_id());
        p->set_type(eventport->get_type());
        p->set_kind((re_common::Port::Kind)((int)eventport->get_kind()));
    }
}
void ModelLogger::LogLifecycleEvent(Component* component, ModelLogger::LifeCycleEvent event){
    auto e = new re_common::LifecycleEvent();
    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    
    e->set_type((re_common::LifecycleEvent::Type)(int)event);
    
    PushMessage(e);
}

void ModelLogger::PushMessage(google::protobuf::MessageLite* message){
    if(active_ && writer_ && message){
        writer_->PushMessage("ModelEvent*", message);
    }else{
        delete message;
    }
}

void ModelLogger::LogLifecycleEvent(EventPort* eventport, ModelLogger::LifeCycleEvent event){
    auto e = new re_common::LifecycleEvent();
    auto component = eventport->get_component();
    //Set info

    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    fill_port(e->mutable_port(), eventport);
    e->set_type((re_common::LifecycleEvent::Type)(int)event);

    PushMessage(e);
}

void ModelLogger::LogWorkerEvent(Worker* worker, std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args){
    auto e = new re_common::WorkloadEvent();

    fill_info(e->mutable_info());
    
    if(worker){
        auto component = worker->get_component();
        if(component){
            fill_component(e->mutable_component(), component);
        }
        //Set Type, Name
        e->set_type(worker->get_worker_name());
        e->set_name(worker->get_name());
        e->set_event_type((re_common::WorkloadEvent::Type)(int)event);
    }

    //Set ID
    if(work_id >= 0){
        e->set_id(work_id);
    }

    if(!function_name.empty()){
        e->set_function(function_name);
    }

    //Set Arg
    if(!args.empty()){
        e->set_args(args);
    }
    

    PushMessage(e);
}

void ModelLogger::LogComponentEvent(EventPort* eventport, ::BaseMessage* message, ModelLogger::ComponentEvent event){
    if(message && eventport){
        auto component = eventport->get_component();
        int ID = message->get_base_message_id();
        auto e = new re_common::ComponentUtilizationEvent();

        fill_info(e->mutable_info());
        fill_component(e->mutable_component(), component);
        fill_port(e->mutable_port(), eventport);

         //Set Type, Name
        e->set_port_event_id(ID);
        e->set_type((re_common::ComponentUtilizationEvent::Type)(int)event);
        PushMessage(e);
    }
}

void ModelLogger::LogMessageEvent(EventPort* eventport){
    //Do Nothing
    auto e = new re_common::MessageEvent();
    auto component = eventport->get_component();
    //Set info

    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    fill_port(e->mutable_port(), eventport);

    PushMessage(e);
}

void ModelLogger::LogUserMessageEvent(Component* component, std::string message){
    auto e = new re_common::UserEvent();
    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);
    
    e->set_type(re_common::UserEvent::MESSAGE);
    e->set_message(message);

    PushMessage(e);
}

void ModelLogger::LogUserFlagEvent(Component* component, std::string message){
    auto e = new re_common::UserEvent();
    fill_info(e->mutable_info());
    fill_component(e->mutable_component(), component);

    e->set_type(re_common::UserEvent::FLAG);
    e->set_message(message);

    

    PushMessage(e);
}