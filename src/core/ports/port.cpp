#include "port.h"
#include "../component.h"
#include "../modellogger.h"

Port::Port(std::weak_ptr<Component> component, const std::string& port_name, const Port::Kind& port_kind, const std::string& port_middleware)
{
    component_ = component;
    set_name(port_name);
    port_kind_ = port_kind;
    port_middleware_ = port_middleware;
}

std::weak_ptr<Component> Port::get_component() const{
    return component_;
};

Port::Kind Port::get_kind() const{
    return port_kind_;
}

std::string Port::get_middleware() const{
    return port_middleware_;
}

void Port::LogActivation(){
    if(logger()){
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
    }
};

void Port::LogPassivation(){
    if(logger()){
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
    }
};

bool Port::HandleActivate(){
    return true;
};

bool Port::HandleConfigure(){
    return true;
};

bool Port::HandlePassivate(){
    return true;
};

bool Port::HandleTerminate(){
    return true;
};

void Port::SetKind(const Port::Kind& port_kind){
    port_kind_ = port_kind;
}

int Port::GetEventsReceived(){
    std::lock_guard<std::mutex> lock(mutex_);
    return received_count_;
}
int Port::GetEventsProcessed(){
    std::lock_guard<std::mutex> lock(mutex_);
    return processed_count_;
}

int Port::GetEventsIgnored(){
    std::lock_guard<std::mutex> lock(mutex_);
    return ignored_count_;
}

void Port::EventRecieved(const BaseMessage& message){
    std::lock_guard<std::mutex> lock(mutex_);
    received_count_ ++;
}

void Port::EventProcessed(const BaseMessage& message, bool processed){
    std::lock_guard<std::mutex> lock(mutex_);
    if(processed){
        processed_count_++;
    }else{
        logger()->LogComponentEvent(*this, message, ModelLogger::ComponentEvent::IGNORED);
        ignored_count_ ++;
    }
}