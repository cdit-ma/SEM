#include "eventport.h"
#include "component.h"
#include "modellogger.h"

EventPort::EventPort(Component* component, std::string name, EventPort::Kind kind){
    set_name(name);
    component_ = component;
    kind_ = kind;

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

bool EventPort::Activate(){
    Activatable::Activate();
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::ACTIVATED);
    return true;
};

bool EventPort::Passivate(){
    Activatable::Passivate();
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::PASSIVATED);
    return true;
};