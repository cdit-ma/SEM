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
    std::lock_guard<std::mutex> lock(port_mutex_);
    //Destory Ports
    eventports_.clear();
}

bool Component::HandleActivate(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    if(BehaviourContainer::HandleActivate()){
        for(const auto& p : eventports_){
            auto& a = p.second;
            if(a){
                a->Activate();
            }
        }
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
        return true;
    }
    return false;
}

bool Component::HandleConfigure(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    if(BehaviourContainer::HandleConfigure()){
        for(const auto& p : eventports_){
            auto& a = p.second;
            if(a){
                a->Configure();
            }
        }
        return true;
    }
    return false;
}

bool Component::HandlePassivate(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    if(BehaviourContainer::HandlePassivate()){
        for(const auto& p : eventports_){
            auto& a = p.second;
            if(a){
                a->Passivate();
            }
        }
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
        return true;
    }
    return false;
}

bool Component::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> state_lock(state_mutex_);

    auto success = false;
    if(BehaviourContainer::HandlePassivate()){
        //Construct a list of async terminate requests.
        //This should lessen the impact of slow destruction of the ports, allowing them to be concurrent
        {
            std::list<std::future<bool> > results;
            std::lock_guard<std::mutex> ports_lock(port_mutex_);

            for(const auto& p : eventports_){
                auto& a = p.second;
                if(a){
                    //Construct a thread to run the terminate function, which is blocking
                    results.push_back(std::async(std::launch::async, &Activatable::Terminate, a));
                }
            }
            //Join all of our termination threads
            for(auto& result : results){
                success &= result.get();
            }
        }
        success = true;
        logger()->LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::TERMINATED);
    }
    return success;
}


std::weak_ptr<EventPort> Component::AddEventPort(std::unique_ptr<EventPort> event_port){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    if(event_port){
        const auto& port_name = event_port->get_name();
        if(eventports_.count(port_name) == 0){
            std::cerr << "Component '" << get_name()  << "' Added an EventPort with name '" << port_name << "'" << std::endl;

            eventports_[port_name] = std::move(event_port);
            return eventports_[port_name];
        }else{
            std::cerr << "Component '" << get_name()  << "' already has an EventPort with name '" << port_name << "'" << std::endl;
        }
    }
    return std::weak_ptr<EventPort>();
}

std::weak_ptr<EventPort> Component::GetEventPort(const std::string& port_name){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    if(eventports_.count(port_name)){
        return eventports_[port_name];
    }
    //std::cerr << "Component '" << get_name() << "' doesn't have an EventPort with name '" << port_name << "'" << std::endl;
    return std::weak_ptr<EventPort>();
}

std::shared_ptr<EventPort> Component::RemoveEventPort(const std::string& port_name){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);

    if(eventports_.count(port_name)){
        auto worker = eventports_[port_name];
        eventports_.erase(port_name);
        return worker;
    }
    std::cerr << "Component '" << get_name() << "' doesn't have an EventPort with name '" << port_name << "'" << std::endl;
    return std::shared_ptr<EventPort>();
}

bool Component::AddCallback_(const std::string& port_name, std::function<void (::BaseMessage&)> function){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    if(callback_functions_.count(port_name) == 0){
        //Store the callback
        callback_functions_[port_name] = function;
        return true;
    }

    std::cerr << "Component '" << get_name() << "' already has a callback with name '" << port_name << "'" << std::endl;
    return false;
}

bool Component::AddPeriodicCallback(const std::string& port_name, std::function<void()> function){
    return AddCallback_(port_name, [function](::BaseMessage& bm){function();});
}

std::function<void (::BaseMessage&)> Component::GetCallback(const std::string& port_name){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    if(callback_functions_.count(port_name)){
        return callback_functions_[port_name];
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << port_name << "'" << std::endl;
    }
    return nullptr;
}

bool Component::RemoveCallback(const std::string& port_name){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    if(callback_functions_.count(port_name) == 0){
        callback_functions_.erase(port_name);
        return true;
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << port_name << "'" << std::endl;
    }
    return false;
}


