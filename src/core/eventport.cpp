#include "eventport.h"
#include "component.h"
#include "modellogger.h"
#include <iostream>
EventPort::EventPort(Component* component, std::string name, EventPort::Kind kind, std::string middleware){
    set_name(name);
    component_ = component;
    kind_ = kind;
    middleware_ = middleware;

    if(component_){
        component_->AddEventPort(this);
    }
};

bool EventPort::IsOutEventPort() const {
    return kind_ == EventPort::Kind::TX;
};

bool EventPort::IsInEventPort() const {
    return kind_ == EventPort::Kind::RX;
};

bool EventPort::IsPeriodicEvent() const {
    return kind_ == EventPort::Kind::PE;
};

Component* EventPort::get_component(){
    return component_;
};

EventPort::Kind EventPort::get_kind() const{
    return kind_;
}

std::string EventPort::get_middleware() const{
    return middleware_;
}

void EventPort::LogActivation(){
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::ACTIVATED);
};

void EventPort::LogPassivation(){
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::PASSIVATED);
};

bool EventPort::Activate(){
    return Activatable::Activate();
};

bool EventPort::Passivate(){
    return Activatable::Passivate();
};

bool EventPort::Teardown(){
    return Activatable::Teardown();
};

void EventPort::SetKind(EventPort::Kind kind){
    kind_ = kind;
}