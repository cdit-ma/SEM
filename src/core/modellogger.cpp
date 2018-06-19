#include "modellogger.h"
#include <ctime>
#include <iostream>

#include "../re_common/zmq/protowriter/cachedprotowriter.h"
#include <proto/modelevent/modelevent.pb.h>

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
    if(writer_){
        delete writer_;
    }
}
const std::string ModelLogger::get_hostname(){
    return host_name_;
}


std::chrono::milliseconds get_current_time(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}


void fill_info(re_common::Info& info){
    info.set_hostname(ModelLogger::get_model_logger()->get_hostname());
    info.set_timestamp(get_current_time().count() / 1000.0);
}

void fill_component(re_common::Component& c, const BehaviourContainer& component){
    c.set_name(component.get_name());
    c.set_id(component.get_id());
    c.set_type(component.get_type());
}


void fill_component(re_common::Component& c, std::weak_ptr<Component>& c_wp){
    auto component = c_wp.lock();
    if(component){
        fill_component(c, *component);
    }
}

void fill_port(re_common::Port& p, const Port& port){
    p.set_name(port.get_name());
    p.set_id(port.get_id());
    p.set_type(port.get_type());
    p.set_kind((re_common::Port::Kind)((int)port.get_kind()));
    p.set_middleware(port.get_middleware());
}

void fill_port(re_common::Port& p, std::weak_ptr<Port>& e_wp){
    auto port = e_wp.lock();
    if(port){
        fill_port(p, *port);
    }
}

void ModelLogger::LogLifecycleEvent(const Component& component, ModelLogger::LifeCycleEvent event){
    auto e = new re_common::LifecycleEvent();
    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);
    
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

void ModelLogger::LogLifecycleEvent(const Port& port, ModelLogger::LifeCycleEvent event){
    auto e = new re_common::LifecycleEvent();
    auto component = port.get_component();
    //Set info

    fill_info(*e->mutable_info());
    
    fill_component(*e->mutable_component(), component);
    fill_port(*e->mutable_port(), port);
    e->set_type((re_common::LifecycleEvent::Type)(int)event);

    PushMessage(e);
}

void ModelLogger::LogWorkerEvent(const Worker& worker, std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args){
    auto e = new re_common::WorkloadEvent();

    const auto& container = worker.get_container();
    
    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), container);

    //Set Type, Name
    e->set_type(worker.get_worker_name());
    e->set_name(worker.get_name());
    e->set_event_type((re_common::WorkloadEvent::Type)(int)event);

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

void ModelLogger::LogComponentEvent(const Port& port, const ::BaseMessage& message, ModelLogger::ComponentEvent event){
    auto component = port.get_component();
    int ID = message.get_base_message_id();
    auto e = new re_common::ComponentUtilizationEvent();

    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);
    fill_port(*e->mutable_port(), port);

        //Set Type, Name
    e->set_port_event_id(ID);
    e->set_type((re_common::ComponentUtilizationEvent::Type)(int)event);
    PushMessage(e);
}

void ModelLogger::LogFailedComponentConstruction(std::string component_type, std::string component_name, std::string component_id){
    auto e = new re_common::LifecycleEvent();
    fill_info(*e->mutable_info());

    auto comp = e->mutable_component();

    comp->set_name(component_name);
    comp->set_type(component_type);
    comp->set_id(component_id);
    
    e->set_type(re_common::LifecycleEvent::FAILED);
    
    PushMessage(e);
}

void ModelLogger::LogFailedPortConstruction(std::string port_type, std::string port_name, std::string port_id){
    auto e = new re_common::LifecycleEvent();
    fill_info(*e->mutable_info());

    auto port = e->mutable_port();

    port->set_name(port_name);
    port->set_type(port_type);
    port->set_id(port_id);
    
    e->set_type(re_common::LifecycleEvent::FAILED);
    
    PushMessage(e);
}

void ModelLogger::LogMessageEvent(const Port& port){
    //Do Nothing
    auto e = new re_common::MessageEvent();
    auto component = port.get_component();
    //Set info

    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);
    fill_port(*e->mutable_port(), port);

    PushMessage(e);
}

void ModelLogger::LogUserMessageEvent(const Component& component, std::string message){
    auto e = new re_common::UserEvent();
    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);
    
    e->set_type(re_common::UserEvent::MESSAGE);
    e->set_message(message);

    PushMessage(e);
}

void ModelLogger::LogUserFlagEvent(const Component& component, std::string message){
    auto e = new re_common::UserEvent();
    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);

    e->set_type(re_common::UserEvent::FLAG);
    e->set_message(message);

    PushMessage(e);
}

Log& Log::Msg(const std::string& message){
    message_ = message;
    return *this;
}

Log& Log::Context(Activatable* context){
    context_ = context;
    return *this;
}

Log& Log::Func(const std::string& function_name){
    function_name_ = function_name;
    return *this;
}

Log& Log::Class(const std::string& class_name){
    class_name_ = class_name;
    return *this;
}

Log::Log(const Severity& severity){
    severity_ = severity;
}

Log::~Log(){
    //TODO NEED TO ACTUALLY IMPLEMENT THIS LOGGING
    auto logger = ModelLogger::get_model_logger();
    if(severity_ < Severity::WARNING){
        std::cerr << "[" << (int)severity_ << "] " << function_name_ << ": " << message_ << std::endl;
    }
}
