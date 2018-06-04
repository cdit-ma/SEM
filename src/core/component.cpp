#include "component.h"
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <future>

#include "modellogger.h"
#include "ports/port.h"
#include "worker.h"

Component::Component(const std::string& component_name){
    set_name(component_name);
}

Component::~Component(){
    Activatable::Terminate();
    std::lock_guard<std::mutex> lock(port_mutex_);
    //Destory Ports
    ports_.clear();
}

bool Component::HandleActivate(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    if(BehaviourContainer::HandleActivate()){
        for(const auto& p : ports_){
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
        for(const auto& p : ports_){
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
        for(const auto& p : ports_){
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

            for(const auto& p : ports_){
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


std::weak_ptr<Port> Component::AddPort(std::unique_ptr<Port> event_port){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    if(event_port){
        const auto& port_name = event_port->get_name();
        if(ports_.count(port_name) == 0){
            std::cerr << "Component '" << get_name()  << "' Added an Port with name '" << port_name << "'" << std::endl;

            ports_[port_name] = std::move(event_port);
            return ports_[port_name];
        }else{
            std::cerr << "Component '" << get_name()  << "' already has an Port with name '" << port_name << "'" << std::endl;
        }
    }
    return std::weak_ptr<Port>();
}

std::weak_ptr<Port> Component::GetPort(const std::string& port_name){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    if(ports_.count(port_name)){
        return ports_[port_name];
    }
    //std::cerr << "Component '" << get_name() << "' doesn't have an Port with name '" << port_name << "'" << std::endl;
    return std::weak_ptr<Port>();
}

std::shared_ptr<Port> Component::RemovePort(const std::string& port_name){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);

    if(ports_.count(port_name)){
        auto worker = ports_[port_name];
        ports_.erase(port_name);
        return worker;
    }
    std::cerr << "Component '" << get_name() << "' doesn't have an Port with name '" << port_name << "'" << std::endl;
    return std::shared_ptr<Port>();
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


