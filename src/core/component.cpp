#include "component.h"
#include <iostream>
#include <thread>

#include "modellogger.h"

Component::Component(std::string inst_name){
    set_name(inst_name);
}

Component::~Component(){
    std::cout << get_name() << "Destructor" << std::endl;

    for(auto it = eventports_.begin(); it != eventports_.end();){
        auto p = it->second;
        std::cout << "DESTROYING PORT: " << p->get_name() << std::endl;
        delete p;
        it = eventports_.erase(it);
    }
}

bool Component::Activate(){
    for(auto e : eventports_){
        e.second->Activate();
        std::cout << "ACTIVATING: " << e.second->get_name() << std::endl;
    }
    Activatable::Activate();
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::ACTIVATED);
    return true;
}

bool Component::Passivate(){
    for(auto e : eventports_){
        e.second->Passivate();
    }
    Activatable::Passivate();

    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::PASSIVATED);
    return true;
}

void Component::AddEventPort(EventPort* event_port){
    if(event_port){
        std::string name = event_port->get_name();
        if(eventports_.count(name) == 0){
            eventports_[name] = event_port;
        }
    }
}

void Component::RemoveEventPort(EventPort* event_port){
    //TODO:
}

void Component::AddAttribute(Attribute* attribute){
    if(attribute){
        if(eventports_.count(attribute->name) == 0){
            attributes_[attribute->name] = attribute;
        }
    }
}

Attribute* Component::GetAttribute(std::string name){
    Attribute* attribute = 0;

    if(attributes_.count(name)){
        attribute = attributes_[name];
    }
    return attribute;
}


std::function<void (::BaseMessage*)> Component::GetCallback(std::string port_name){
    std::function<void (::BaseMessage*)> f = NULL;
    if(callback_functions_.count(port_name)){
        f = callback_functions_[port_name];
    }else{
        std::cout << "No Callbacks for: " << port_name << " !" << std::endl;
    }
    return f;
}

void Component::AddCallback(std::string port_name, std::function<void (::BaseMessage*)> function){
    if(callback_functions_.count(port_name) == 0){
        std::cout << "Component: " << get_name() << " Added Callback function for: " << port_name << std::endl;
        callback_functions_[port_name] = function;
    }
}


EventPort* Component::GetEventPort(std::string name){
    EventPort* port = 0;
    if(eventports_.count(name)){
        port = eventports_[name];
    }
    return port;
}