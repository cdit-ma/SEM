
#include "logan_logger.h"
#include <proto/modelevent/modelevent.pb.h>

#include <core/ports/port.h>
#include <core/component.h>
#include <core/worker.h>

void FillInfoPB(re_common::Info& info, Logan::Logger& logger){
    info.set_experiment_name(logger.GetExperimentName());
    info.set_hostname(logger.GetHostName());
    info.set_timestamp(logger.GetCurrentTime().count() / 1000.0);
}

void FillComponentPB(re_common::Component& c, const Component& component){
    c.set_name(component.GetLocalisedName());
    c.set_id(component.get_id());
    c.set_type(component.get_type());
}

void FillWorkerPB(re_common::Worker& w, const Worker& worker){
    w.set_name(worker.get_name());
    w.set_id(worker.get_id());
    w.set_type(worker.get_worker_name());
}

void FillPortPB(re_common::Port& p, const Port& port){
    p.set_name(port.get_name());
    p.set_id(port.get_id());
    p.set_type(port.get_type());
    p.set_kind((re_common::Port::Kind)((int)port.get_kind()));
    p.set_middleware(port.get_middleware());
}

Logan::Logger::Logger(const std::string& experiment_name, const std::string& host_name, const std::string& address, const std::string& port, Logger::Mode mode):
    experiment_name_(experiment_name),
    host_name_(host_name)
{
    const auto& endpoint = "tcp://" + address + ":" + port;
        
    switch(mode){
        case Mode::LIVE:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
            break;
        }
        case Mode::CACHED:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
            break;
        }
        default:{
            throw std::runtime_error("Invalid logging Mode");
        }
    }
    if(!writer_->BindPublisherSocket(endpoint)){
        throw std::runtime_error("Cannot bind endpoint: " + endpoint);
    }
}

void Logan::Logger::LogMessage(const Activatable& entity, bool is_exception, const std::string& message){
    auto event_pb = std::unique_ptr<re_common::UtilizationEvent>(new re_common::UtilizationEvent());
    event_pb->set_type(is_exception ? re_common::UtilizationEvent::EXCEPTION : re_common::UtilizationEvent::MESSAGE);
    if(message.size()){
        event_pb->set_message(message);
    }

    FillInfoPB(*(event_pb->mutable_info()), *this);
    
    if(entity.get_class() == Activatable::Class::COMPONENT){
        const auto& component = (const Component&)entity;
        FillComponentPB(*(event_pb->mutable_component()), component);
    }else if(entity.get_class() == Activatable::Class::PORT){
        const auto& port = (const Port&)entity;
        FillPortPB(*(event_pb->mutable_port()), port);

        auto component = port.get_component().lock();
        if(component){
            FillComponentPB(*(event_pb->mutable_component()), *component);
        }
    }else{
        return;
    }

    PushMessage(std::move(event_pb));
}

void Logan::Logger::LogMessage(const Activatable& entity, const std::string& message){
    LogMessage(entity, false, message);
}

void Logan::Logger::LogException(const Activatable& entity, const std::string& message){
    LogMessage(entity, true, message);
}

void Logan::Logger::LogWorkerEvent(const Worker& worker, const std::string& function_name, const ::Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){
    auto event_pb = std::unique_ptr<re_common::WorkloadEvent>(new re_common::WorkloadEvent());
    event_pb->set_event_type((re_common::WorkloadEvent::Type)(int)event);

    FillInfoPB(*(event_pb->mutable_info()), *this);

    auto log_level = GetWorkloadLogLevel(event, message_log_level);

    
    try{
        auto& component = worker.get_component();
        FillComponentPB(*(event_pb->mutable_component()), component);
    }catch(const std::exception& e){
    }

    FillWorkerPB(*(event_pb->mutable_worker()), worker);
    
    if(work_id >= 0){
        event_pb->set_workload_id(work_id);
    }

    if(function_name.size()){
        event_pb->set_function_name(function_name);
    }

    event_pb->set_log_level(log_level);

    if(args.size()){
        event_pb->set_args(args);
    }

    PushMessage(std::move(event_pb));
}

void Logan::Logger::LogLifecycleEvent(const Activatable& entity, const ::Logger::LifeCycleEvent& event){
    auto event_pb = std::unique_ptr<re_common::LifecycleEvent>(new re_common::LifecycleEvent());
    
    event_pb->set_type((re_common::LifecycleEvent::Type)(int)event);
    FillInfoPB(*(event_pb->mutable_info()), *this);

    if(entity.get_class() == Activatable::Class::COMPONENT){
        const auto& component = (const Component&)entity;
        FillComponentPB(*(event_pb->mutable_component()), component);
    }else if(entity.get_class() == Activatable::Class::PORT){
        const auto& port = (const Port&)entity;
        FillPortPB(*(event_pb->mutable_port()), port);
        auto component = port.get_component().lock();
        if(component){
            FillComponentPB(*(event_pb->mutable_component()), *component);
        }
    }else{
        return;
    }

    PushMessage(std::move(event_pb)); 
}

void Logan::Logger::LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const ::Logger::UtilizationEvent& event, const std::string& message_str){
    auto event_pb = std::unique_ptr<re_common::UtilizationEvent>(new re_common::UtilizationEvent());
    
    event_pb->set_type((re_common::UtilizationEvent::Type)(int)event);
    
    FillInfoPB(*(event_pb->mutable_info()), *this);
    FillPortPB(*(event_pb->mutable_port()), port);

    auto component = port.get_component().lock();
    if(component){
        FillComponentPB(*(event_pb->mutable_component()), *component);
    }

    event_pb->set_port_event_id(message.get_base_message_id());
    
    if(message_str.size()){
        event_pb->set_message(message_str);
    }

    PushMessage(std::move(event_pb)); 
}

std::chrono::milliseconds Logan::Logger::GetCurrentTime(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

void Logan::Logger::PushMessage(std::unique_ptr<google::protobuf::MessageLite> message){
    if(writer_){
        writer_->PushMessage("ModelEvent*", std::move(message));
    }
}
const std::string& Logan::Logger::GetExperimentName() const{
    std::lock_guard<std::mutex> lock(mutex_);
    return experiment_name_;
}

const std::string& Logan::Logger::GetHostName() const{
    std::lock_guard<std::mutex> lock(mutex_);
    return host_name_;
}