#include "component.h"
#include <iostream>

Component::Component(std::string inst_name){
    set_name(inst_name);
}

Component::~Component(){

    std::cout <<get_name() << "Destructor" << std::endl;
}

bool Component::activate(){
    for(auto e : eventports_){
        e.second->activate();
    }
    Activatable::activate();
    return true;
}

bool Component::passivate(){
    for(auto e : eventports_){
        e.second->passivate();
    }
    Activatable::passivate();
    return true;
}

void Component::add_event_port(EventPort* event_port){
    if(event_port){
        std::string name = event_port->get_name();
        if(eventports_.count(name) == 0){
            std::cout << "Component: " << get_name() << " Added Port: " << name << std::endl;
            eventports_[name] = event_port;
        }
    }
}

void Component::remove_event_port(EventPort* event_port){
    //TODO:
}

void Component::add_attribute(Attribute* attribute){
    if(attribute){
        if(eventports_.count(attribute->name) == 0){
            std::cout << "Component: " << get_name() << " Added Attribute: " << attribute->name << std::endl;
            attributes_[attribute->name] = attribute;
        }
    }
}

Attribute* Component::get_attribute(std::string name){
    Attribute* attribute = 0;

    if(attributes_.count(name)){
        attribute = attributes_[name];
    }
    return attribute;
}

EventPort* Component::get_event_port(std::string name){
    EventPort* port = 0;
    if(eventports_.count(name)){
        port = eventports_[name];
    }
    return port;
}