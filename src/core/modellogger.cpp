#include "modellogger.h"
#include <ctime>
#include <iostream>

#include <proto/modelevent/modelevent.pb.h>
#include <zmq/protowriter/cachedprotowriter.h>

#include "component.h"
#include "ports/port.h"
#include "worker.h"

void ErrorPrint_ComponentUtilizationEvent(const re_common::ComponentUtilizationEvent& event){
    static std::mutex cerr_mutex;
    std::lock_guard<std::mutex> lock(cerr_mutex);

    std::cerr << "[" << event.info().hostname() << "/";
    std::cerr << event.component().name() << "<" << event.component().type() << ">/";
    std::cerr << event.port().name() << "<" << event.port().type() << ">";
    std::cerr << "] Exception: " << event.message();
    std::cerr << std::endl;
}

void Print_WorkerEvent(const re_common::WorkloadEvent& event){
    static std::mutex cout_mutex;
    std::lock_guard<std::mutex> lock(cout_mutex);

    std::cout << "[" << event.info().hostname() << "/";
    std::cout << event.component().name() << "<" << event.component().type() << ">/";
    std::cout << event.name() << "/" << event.function() << "";
    std::cout << "] " << event.args();
    std::cout << std::endl;
}

bool ModelLogger::setup_model_logger(const std::string& experiment_name, const std::string& host_name, const std::string& address, const std::string& port, Mode mode){
    auto& s = get_model_logger();
    if(!s.is_setup()){
        auto endpoint = "tcp://" + address + ":" + port;
        s.setup_logger(experiment_name, host_name, endpoint, mode);
        return true;
    }
    return false;
}

ModelLogger& ModelLogger::get_model_logger(){
    static std::unique_ptr<ModelLogger> logger_(new ModelLogger());
    return *logger_;
}

bool ModelLogger::is_logger_setup(){
    return get_model_logger().is_setup();
}

bool ModelLogger::shutdown_logger(){
    auto& s = get_model_logger();
    std::lock_guard<std::mutex> lock(s.mutex_);
    if(s.is_setup()){
        s.writer_.reset();
        return true;
    }
    return false;
}

bool ModelLogger::is_setup(){
    return writer_ != nullptr;
}

void ModelLogger::setup_logger(const std::string& experiment_name, const std::string& host_name, const std::string& endpoint, Mode mode){
    host_name_ = host_name;
    experiment_name_ = experiment_name;
    switch(mode){
        case Mode::LIVE:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
            break;
        }
        case Mode::CACHED:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
            break;
        }
        default:
            break;
    }
    if(writer_){
        if(!writer_->BindPublisherSocket(endpoint)){
            throw std::runtime_error("Cannot bind endpoint: " + endpoint);
        }
    }
}

const std::string& ModelLogger::get_hostname() const{
    return host_name_;
}

const std::string& ModelLogger::get_experiment_name() const{
    return experiment_name_;
}



std::chrono::milliseconds get_current_time(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}


void fill_info(re_common::Info& info){
    info.set_experiment_name(ModelLogger::get_model_logger().get_experiment_name());
    info.set_hostname(ModelLogger::get_model_logger().get_hostname());
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

void ModelLogger::LogWorkerEvent(const Worker& worker, std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args, bool print){
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
    
    if(print){
        Print_WorkerEvent(*e);
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




void ModelLogger::LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string, bool print){
    

    auto component = port.get_component();
    int ID = message.get_base_message_id();
    auto e = new re_common::ComponentUtilizationEvent();

    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);
    fill_port(*e->mutable_port(), port);

    e->set_port_event_id(ID);
    e->set_type(re_common::ComponentUtilizationEvent::EXCEPTION);
    e->set_message(error_string);

    if(print){
        ErrorPrint_ComponentUtilizationEvent(*e);
    }
    PushMessage(e);
}



void ModelLogger::LogPortExceptionEvent(const Port& port, const std::string& error_string, bool print){
    auto component = port.get_component();
    auto e = new re_common::ComponentUtilizationEvent();

    fill_info(*e->mutable_info());
    fill_component(*e->mutable_component(), component);
    fill_port(*e->mutable_port(), port);
    
    e->set_type(re_common::ComponentUtilizationEvent::EXCEPTION);
    e->set_message(error_string);

    if(print){
        ErrorPrint_ComponentUtilizationEvent(*e);
    }
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
    auto& logger = ModelLogger::get_model_logger();
    if(severity_ < Severity::WARNING){
        std::cerr << "[" << (int)severity_ << "] " << function_name_ << ": " << message_ << std::endl;
    }
}
