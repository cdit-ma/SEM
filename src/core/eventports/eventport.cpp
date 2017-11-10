#include "eventport.h"
#include "../component.h"
#include "../modellogger.h"

EventPort::EventPort(Component* component, std::string name, EventPort::Kind kind, std::string middleware):
    EventPort::EventPort(std::shared_ptr<Component>(component), name, kind, middleware){

    };

EventPort::EventPort(std::shared_ptr<Component> component, std::string name, EventPort::Kind kind, std::string middleware){
    set_name(name);
    component_ = component;
    kind_ = kind;
    middleware_ = middleware;
}

std::shared_ptr<Component> EventPort::get_component(){
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
