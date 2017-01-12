#include "component.h"
#include <iostream>

Component::Component(std::string inst_name){
    inst_name_ = inst_name;
}

const std::string Component::get_name(){
    return this->inst_name_;
}

bool Component::activate(){
    for(auto e : eventports_){
        std::cout << e << std::endl;
        e->activate();
    }
    Activatable::activate();
    return true;
}

bool Component::passivate(){
    for(auto e : eventports_){
        e->passivate();
    }
    Activatable::passivate();
    return true;
}

void Component::add_event_port(EventPort* event_port){
    eventports_.push_back(event_port);
}

void Component::remove_event_port(EventPort* event_port){
    //TODO:
}