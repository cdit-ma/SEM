#include "component.h"
#include <iostream>
#include <thread>
#include <algorithm>

#include "modellogger.h"
#include "eventports/eventport.h"
#include "worker.h"

int get_eventport_order(const std::shared_ptr<EventPort> a){
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

bool compare_eventport(const std::shared_ptr<EventPort> a, const std::shared_ptr<EventPort> b){
    return get_eventport_order(a) < get_eventport_order(b);
}


Component::Component(std::string inst_name){
    set_name(inst_name);
}

Component::~Component(){
    Activatable::Terminate();
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Destory Ports
    eventports_.clear();
    workers_.clear();
}

bool Component::HandleActivate(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    for(auto &p : workers_){
        auto a = p.second;
        if(a){
            a->Activate();
        }
    }
    for(auto &p : eventports_){
        auto a = p.second;
        if(a){
            a->Activate();
        }
    }
    logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
    return true;
}

bool Component::HandlePassivate(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    for(auto &p : workers_){
        auto a = p.second;
        if(a){
            a->Passivate();
        }
    }
    for(auto &p : eventports_){
        auto a = p.second;
        if(a){
            a->Passivate();
        }
    }
    //Log message
    logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
    return true;
}

bool Component::HandleTerminate(){
    HandlePassivate();

    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    std::lock_guard<std::mutex> lock2(mutex_);
    for(auto& p : workers_){
        auto a = p.second;
        if(a){
            a->Terminate();
        }
    }
    for(auto& p : eventports_){
        auto a = p.second;
        if(a){
            a->Terminate();
        }
    }
    return true;
}

bool Component::HandleConfigure(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    for(auto &p : workers_){
        auto a = p.second;
        if(a){
            auto success = a->Configure();
            if(!success){
                std::cerr << get_name() << " " << a->get_name() << std::endl;
            }
        }
    }
    for(auto &p : eventports_){
        auto a = p.second;
        if(a){
            auto success = a->Configure();
            if(!success){
                std::cerr << get_name() << " " << a->get_name() << std::endl;
            }
        }
    }
    return true;
}

std::weak_ptr<EventPort> Component::AddEventPort(std::unique_ptr<EventPort> event_port){
    std::lock_guard<std::mutex> lock(mutex_);
    if(event_port){
        std::string name = event_port->get_name();
        if(eventports_.count(name) == 0){
            eventports_[name] = std::move(event_port);
            return eventports_[name];
        }else{
            std::cerr << "Component '" << get_name()  << "' already has an EventPort with name '" << name << "'" << std::endl;
        }
    }else{
        std::cerr << "Component '" << get_name()  << "' cannot add a null EventPort" << std::endl;
    }
    return std::weak_ptr<EventPort>();
}

std::weak_ptr<EventPort> Component::GetEventPort(const std::string& name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(eventports_.count(name)){
        return eventports_[name];
    }else{
        //std::cerr << "Component '" << get_name()  << "' doesn't has an EventPort with name '" << name << "'" << std::endl;
    }
    return std::weak_ptr<EventPort>();
}

std::shared_ptr<EventPort> Component::RemoveEventPort(const std::string& name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(eventports_.count(name)){
        auto port = eventports_[name];
        eventports_.erase(name);
        return port;
    }else{ 
        std::cerr << "Component '" << get_name()  << "' doesn't has an EventPort with name '" << name << "'" << std::endl;
        return std::shared_ptr<EventPort>();
    }
}


std::weak_ptr<Worker> Component::AddWorker(std::unique_ptr<Worker> worker){
    std::lock_guard<std::mutex> lock(mutex_);
    if(worker){
        std::string name = worker->get_name();
        if(workers_.count(name) == 0){
            workers_[name] = std::move(worker);
            return workers_[name];
        }else{
            std::cerr << "Component '" << get_name()  << "' already has an Worker with name '" << name << "'" << std::endl;
        }
    }else{
        std::cerr << "Component '" << get_name()  << "' cannot add a null Worker" << std::endl;
    }
    return std::weak_ptr<Worker>();
}

std::weak_ptr<Worker> Component::GetWorker(const std::string& name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(workers_.count(name)){
        return workers_[name];
    }else{
        std::cerr << "Component '" << get_name() << "' doesn't have an Worker with name '" << name << "'" << std::endl;
    }
    return std::weak_ptr<Worker>();
}


std::shared_ptr<Worker> Component::RemoveWorker(const std::string& name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(workers_.count(name)){
        auto worker = workers_[name];
        workers_.erase(name);
        return worker;
    }else{
        std::cerr << "Component '" << get_name() << "' doesn't have an Worker with name '" << name << "'" << std::endl;
    }
    return std::shared_ptr<Worker>();
}

void Component::AddCallback(const std::string& name, std::function<void (::BaseMessage*)> function){
    //auto port = GetEventPort(port_name);
    std::lock_guard<std::mutex> lock(mutex_);
    
    if(callback_functions_.count(name) == 0){
        //Store the callback
        callback_functions_[name] = function;
    }
}

std::function<void (::BaseMessage*)> Component::GetCallback(const std::string& name){
    std::lock_guard<std::mutex> lock(mutex_);
    std::function<void (::BaseMessage*)> f;
    if(callback_functions_.count(name)){
        f = callback_functions_[name];
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << name << "'" << std::endl;
    }
    return f;
}

bool Component::RemoveCallback(const std::string& name){
    //auto port = GetEventPort(port_name);
    std::lock_guard<std::mutex> lock(mutex_);
    
    if(callback_functions_.count(name) == 0){
        callback_functions_.erase(name);
        return true;
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << name << "'" << std::endl;
    }
    return false;
}


