#include "component.h"
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <future>

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


Component::Component(const std::string& component_name){
    set_name(component_name);
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
    for(const auto& p : workers_){
        auto& a = p.second;
        if(a){
            a->Activate();
        }
    }
    for(const auto& p : eventports_){
        auto& a = p.second;
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
    for(const auto& p : workers_){
        auto& a = p.second;
        if(a){
            a->Passivate();
        }
    }
    for(const auto& p : eventports_){
        auto& a = p.second;
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
    std::lock_guard<std::mutex> lock(state_mutex_);

    auto success = true;
    std::list<std::future<bool> > results;
    //Construct a list of async terminate requests.
    //This should lessen the impact of slow destruction of the ports, allowing them to be concurrent
    {
        std::lock_guard<std::mutex> lock2(element_mutex_);
        for(const auto& p : workers_){
            auto& a = p.second;
            if(a){
                //Construct a thread to run the terminate function, which is blocking
                results.push_back(std::async(&Activatable::Terminate, a));
            }
        }
        for(const auto& p : eventports_){
            auto& a = p.second;
            if(a){
                //Construct a thread to run the terminate function, which is blocking
                results.push_back(std::async(&Activatable::Terminate, a));
            }
        }
    }

    //Join all of our termination threads
    for(auto& result : results){
        success &= result.get();
    }

    return true;
}

bool Component::HandleConfigure(){
    //Gain mutex lock
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    for(const auto& p : workers_){
        auto& a = p.second;
        if(a){
            auto success = a->Configure();
            if(!success){
                std::cerr << get_name() << " " << a->get_name() << std::endl;
            }
        }
    }
    for(const auto& p : eventports_){
        auto& a = p.second;
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
    //Stop concurrent Mutators on the Component
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock2(element_mutex_);
    
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

std::weak_ptr<EventPort> Component::GetEventPort(const std::string& event_port_name){
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock(element_mutex_);
    if(eventports_.count(event_port_name)){
        return eventports_[event_port_name];
    }
    return std::weak_ptr<EventPort>();
}

std::shared_ptr<EventPort> Component::RemoveEventPort(const std::string& event_port_name){
    //Stop concurrent Mutators on the Component
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock2(element_mutex_);
    if(eventports_.count(event_port_name)){
        auto port = eventports_[event_port_name];
        eventports_.erase(event_port_name);
        return port;
    }else{ 
        std::cerr << "Component '" << get_name()  << "' doesn't has an EventPort with name '" << event_port_name << "'" << std::endl;
        return std::shared_ptr<EventPort>();
    }
}


std::weak_ptr<Worker> Component::AddWorker(std::unique_ptr<Worker> worker){
    //Stop concurrent Mutators on the Component
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock2(element_mutex_);
    if(worker){
        const auto& worker_name = worker->get_name();
        if(workers_.count(worker_name) == 0){
            workers_[worker_name] = std::move(worker);
            return workers_[worker_name];
        }else{
            std::cerr << "Component '" << get_name()  << "' already has an Worker with name '" << worker_name << "'" << std::endl;
        }
    }else{
        std::cerr << "Component '" << get_name()  << "' cannot add a null Worker" << std::endl;
    }
    return std::weak_ptr<Worker>();
}

std::weak_ptr<Worker> Component::GetWorker(const std::string& worker_name){
    std::lock_guard<std::mutex> lock(element_mutex_);
    if(workers_.count(worker_name)){
        return workers_[worker_name];
    }else{
        std::cerr << "Component '" << get_name() << "' doesn't have an Worker with name '" << worker_name << "'" << std::endl;
    }
    return std::weak_ptr<Worker>();
}


std::shared_ptr<Worker> Component::RemoveWorker(const std::string& worker_name){
    //Stop concurrent Mutators on the Component
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock2(element_mutex_);
    if(workers_.count(worker_name)){
        auto worker = workers_[worker_name];
        workers_.erase(worker_name);
        return worker;
    }else{
        std::cerr << "Component '" << get_name() << "' doesn't have an Worker with name '" << worker_name << "'" << std::endl;
    }
    return std::shared_ptr<Worker>();
}

bool Component::AddCallback_(const std::string& event_port_name, std::function<void (::BaseMessage&)> function){
    //Stop concurrent Mutators on the Component
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock2(element_mutex_);
    
    if(callback_functions_.count(event_port_name) == 0){
        //Store the callback
        callback_functions_[event_port_name] = function;
    }else{
        std::cerr << "Component '" << get_name() << "' already has a callback with name '" << event_port_name << "'" << std::endl;
        return false;
    }
    return true;
}
bool Component::AddPeriodicCallback(const std::string& event_port_name, std::function<void()> function){
    return AddCallback_(event_port_name, [function](::BaseMessage& bm){function();});
}

std::function<void (::BaseMessage&)> Component::GetCallback(const std::string& event_port_name){
    std::lock_guard<std::mutex> lock(element_mutex_);
    if(callback_functions_.count(event_port_name)){
        return callback_functions_[event_port_name];
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << event_port_name << "'" << std::endl;
    }
    return nullptr;
}

bool Component::RemoveCallback(const std::string& event_port_name){
    //Stop concurrent Mutators on the Component
    std::lock_guard<std::mutex> lock(state_mutex_);
    //Stop reading accessors whilst modifying elements
    std::lock_guard<std::mutex> lock2(element_mutex_);
    
    if(callback_functions_.count(event_port_name) == 0){
        callback_functions_.erase(event_port_name);
        return true;
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << event_port_name << "'" << std::endl;
    }
    return false;
}


