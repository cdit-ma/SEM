#include "eventport.h"
#include "../component.h"
#include "../modellogger.h"

EventPort::EventPort(std::weak_ptr<Component> component, const std::string& port_name, const EventPort::Kind& port_kind, const std::string& port_middleware)
{
    component_ = component;
    set_name(port_name);
    port_kind_ = port_kind;
    port_middleware_ = port_middleware;
}

std::weak_ptr<Component> EventPort::get_component() const{
    return component_;
};

EventPort::Kind EventPort::get_kind() const{
    return port_kind_;
}

std::string EventPort::get_middleware() const{
    return port_middleware_;
}

void EventPort::LogActivation(){
    if(logger()){
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
    }
};

void EventPort::LogPassivation(){
    if(logger()){
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
    }
};

bool EventPort::HandleActivate(){
    return true;
};

bool EventPort::HandleConfigure(){
    return true;
};

bool EventPort::HandlePassivate(){
    return true;
};

bool EventPort::HandleTerminate(){
    return true;
};

void EventPort::SetKind(const EventPort::Kind& port_kind){
    port_kind_ = port_kind;
}

int EventPort::GetEventsReceived(){
    std::lock_guard<std::mutex> lock(mutex_);
    return received_count_;
}
int EventPort::GetEventsProcessed(){
    std::lock_guard<std::mutex> lock(mutex_);
    return processed_count_;
}

int EventPort::GetEventsIgnored(){
    std::lock_guard<std::mutex> lock(mutex_);
    return ignored_count_;
}

void EventPort::EventRecieved(const BaseMessage& message){
    std::lock_guard<std::mutex> lock(mutex_);
    received_count_ ++;
}

void EventPort::EventProcessed(const BaseMessage& message, bool processed){
    std::lock_guard<std::mutex> lock(mutex_);
    if(processed){
        processed_count_++;
    }else{
        logger()->LogComponentEvent(*this, message, ModelLogger::ComponentEvent::IGNORED);
        ignored_count_ ++;
    }
}