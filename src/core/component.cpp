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
    Passivate();
    Teardown();
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Destory Ports
    eventports_.clear();
    workers_.clear();
}

bool Component::Activate(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(Activatable::Activate()){
        //Get the ports
        for(auto itt : eventports_){
            auto e = itt.second; 
            //std::cout << "Activating: " << e->get_name() << std::endl;
            e->Activate();
        }
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
        return true;
    }
    return false;
}

bool Component::Passivate(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(Activatable::Passivate()){
        for(auto itt : eventports_){
            auto e = itt.second;
            bool success = e->Passivate();
        }
        //Log message
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
        return true;
    }
    return false;
}

bool Component::Teardown(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(Activatable::Terminate()){
        for(auto e : eventports_){
            auto eventport = e.second;
            eventport->Terminate();
        }
        return true;
    }
    return false;
}

void Component::AddEventPort(std::shared_ptr<EventPort> event_port){
    std::lock_guard<std::mutex> lock(mutex_);
    if(event_port){
        std::string name = event_port->get_name();
        if(eventports_.count(name) == 0){
            eventports_[name] = event_port;
        }

    }
}

void Component::AddWorker(std::shared_ptr<Worker> worker){
    std::lock_guard<std::mutex> lock(mutex_);
    if(worker){
        std::string name = worker->get_name();
        if(workers_.count(name) == 0){
            workers_[name] = worker;
        }
    }
}

std::shared_ptr<Worker> Component::GetWorker(std::string name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(workers_.count(name)){
        return workers_[name];
    }
    return nullptr;
}


void Component::RemoveEventPort(std::shared_ptr<EventPort> event_port){
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

std::shared_ptr<EventPort> Component::GetEventPort(std::string name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(eventports_.count(name)){
        return eventports_[name];
    }
    return nullptr;
}
