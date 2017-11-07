#include "component.h"
#include <iostream>
#include <thread>
#include <algorithm>

#include "modellogger.h"
#include "eventport.h"
#include "worker.h"

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
    Passivate();
    Teardown();
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Destory Ports
    for(auto it = eventports_.begin(); it != eventports_.end();){
        auto p = it->second;
        if(p){
            delete p;
        }
        it = eventports_.erase(it);
    }
    //Destory Workers
    for(auto it = workers_.begin(); it != workers_.end();){
        auto p = it->second;
        if(p){
            delete p;
        }
        it = workers_.erase(it);
    }
}

bool Component::Activate(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(Activatable::Activate()){
        //Get the ports
        auto ports = GetSortedPorts(true);
        for(auto e : ports){
            //std::cout << "Activating: " << e->get_name() << std::endl;
            e->Activate();
        }
        logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::ACTIVATED);
        return true;
    }
    return false;
}

bool Component::Passivate(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(Activatable::Passivate()){
        auto ports = GetSortedPorts(false);
        for(auto e : ports){
            bool success = e->Passivate();
        }
        //Log message
        logger()->LogLifecycleEvent(this, ModelLogger::LifeCycleEvent::PASSIVATED);
        return true;
    }
    return false;
}

bool Component::Teardown(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(Activatable::Teardown()){
        for(auto e : eventports_){
            auto eventport = e.second;
            eventport->Teardown();
        }
        return true;
    }
    return false;
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

void Component::AddWorker(Worker* worker){
    std::lock_guard<std::mutex> lock(mutex_);
    if(worker){
        std::string name = worker->get_name();
        if(workers_.count(name) == 0){
            workers_[name] = worker;
        }
    }
}

Worker* Component::GetWorker(std::string name){
    std::lock_guard<std::mutex> lock(mutex_);
    Worker* worker = 0;
    
    if(workers_.count(name)){
        worker = workers_[name];
    }
    return worker;
}

std::vector<EventPort*> Component::GetSortedPorts(bool forward){
    std::vector<EventPort*> eventports;
    {
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
