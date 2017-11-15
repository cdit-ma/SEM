#include "eventport.h"
#include "../component.h"
#include "../modellogger.h"

EventPort::EventPort(std::weak_ptr<Component> component, std::string name, EventPort::Kind kind, std::string middleware)
{
    component_ = component;
    set_name(name);
    kind_ = kind;
    middleware_ = middleware;
}

std::weak_ptr<Component> EventPort::get_component() const{
    return component_;
};

EventPort::Kind EventPort::get_kind() const{
    return kind_;
}

std::string EventPort::get_middleware() const{
    return middleware_;
}

void EventPort::LogActivation(){
    logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
};

void EventPort::LogPassivation(){
    logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
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

void EventPort::SetKind(EventPort::Kind kind){
    kind_ = kind;
}
