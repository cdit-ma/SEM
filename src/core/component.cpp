#include "component.h"
#include <iostream>
#include <thread>

#include "modellogger.h"

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
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto e : eventports_){
        e.second->Activate();
    }
    Activatable::Activate();
    if(logger()){
        logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::ACTIVATED);
    }
    return true;
}

bool Component::Passivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto e : eventports_){
        e.second->Passivate();
    }
    Activatable::Passivate();
    if(logger()){
        logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::PASSIVATED);
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
        
        //Wrap the provided callback function; Facilitates logging and memory allocation
       /* auto logged_callback_ = [this, function, port](BaseMessage* m){
            if(function){
                //Log Start
                logger()->LogUtilizationEvent(port, m, ModelLogger::WorkloadEvent::STARTED);
                //Run call back
                function(m);
                //Log Finish
                logger()->LogUtilizationEvent(port, m, ModelLogger::WorkloadEvent::FINISHED);
            }
            //Free Memory
            delete m;
        };*/
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
