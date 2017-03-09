#include "component.h"
#include <iostream>
#include <thread>
#include <algorithm>

#include "modellogger.h"

int get_eventport_order(const EventPort* a){
    //Required Order:
    //1. InEventPorts
    //2. OutEventPorts
    //3. PeriodicEventPorts
    switch(a->get_kind()){
        case EventPort::Kind::TX:
            return 1;
        case EventPort::Kind::RX:
            return 0;
        case EventPort::Kind::PE:
            return 2;
        default:
            return 3;
    }
}

bool compare_eventport(const EventPort* a, const EventPort* b){
    return get_eventport_order(a) < get_eventport_order(b);
}


Component::Component(std::string inst_name){
    set_name(inst_name);
}

Component::~Component(){
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto it = eventports_.begin(); it != eventports_.end();){
        auto p = it->second;
        if(p){
            delete p;
        }
        it = eventports_.erase(it);
    }
}

bool Component::Activate(){
    {
        auto ports = GetSortedPorts(true);
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto e : ports){
            e->Activate();
        }
        Activatable::Activate();
    }
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::ACTIVATED);
    return true;
}

bool Component::Passivate(){
    {
        auto ports = GetSortedPorts(false);
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto e : ports){
            e->Passivate();
        }
        Activatable::Passivate();
    }
    logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::PASSIVATED);

    return true;
}

bool Component::Teardown(){
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto e : eventports_){
            e.second->Teardown();
        }
    }
    return true;
}

void Component::AddEventPort(EventPort* event_port){
    std::lock_guard<std::mutex> lock(mutex_);
    if(event_port){
        std::string name = event_port->get_name();
        if(eventports_.count(name) == 0){
            eventports_[name] = event_port;
        }

    }
}

std::vector<EventPort*> Component::GetSortedPorts(bool forward){
    std::vector<EventPort*> eventports;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for(auto e : eventports_){
            eventports.push_back(e.second);
        }
    }

    //Sort forward or backward
    if(forward){
        std::sort(eventports.begin(), eventports.end(), compare_eventport);
    }else{
        std::sort(eventports.rbegin(), eventports.rend(), compare_eventport);
    }

    return eventports;
}

void Component::RemoveEventPort(EventPort* event_port){
    //TODO:
}

void Component::AddAttribute(Attribute* attribute){
    std::lock_guard<std::mutex> lock(mutex_);
    if(attribute){
        auto name = attribute->get_name();
        if(eventports_.count(name) == 0){
            attributes_[name] = attribute;
        }
    }
}

Attribute* Component::GetAttribute(std::string name){
    std::lock_guard<std::mutex> lock(mutex_);
    Attribute* attribute = 0;
    
    if(attributes_.count(name)){
        attribute = attributes_[name];
    }
    return attribute;
}


std::function<void (::BaseMessage*)> Component::GetCallback(std::string port_name){
    std::lock_guard<std::mutex> lock(mutex_);
    std::function<void (::BaseMessage*)> f = NULL;
    if(callback_functions_.count(port_name)){
        f = callback_functions_[port_name];
    }else{
        std::cout << "No Callbacks for: " << port_name << " !" << std::endl;
    }
    return f;
}

void Component::AddCallback(std::string port_name, std::function<void (::BaseMessage*)> function){
    auto port = GetEventPort(port_name);
    std::lock_guard<std::mutex> lock(mutex_);
    
    if(callback_functions_.count(port_name) == 0){
        //Store the callback
        callback_functions_[port_name] = function;
    }
}

EventPort* Component::GetEventPort(std::string name){
    std::lock_guard<std::mutex> lock(mutex_);
    EventPort* port = 0;
    if(eventports_.count(name)){
        port = eventports_[name];
    }
    return port;
}
