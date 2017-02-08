#include "eventport.h"
#include "component.h"

EventPort::EventPort(Component* component, std::string name, EventPort::Type type){
    set_name(name);
    component_ = component;
    type_ = type;

    if(component_){
        component_->AddEventPort(this);
    }
};


const bool EventPort::IsOutEventPort(){
    return type_ == EventPort::Type::TX;
};

const bool EventPort::IsInEventPort(){
    return type_ == EventPort::Type::RX;
};

const bool EventPort::IsPeriodicEvent(){
    return type_ == EventPort::Type::PE;
};

Component* EventPort::get_component(){
    return component_;
}