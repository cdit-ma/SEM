#include "component.h"
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <future>
#include <sstream>

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

void Component::HandleActivate(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    for(const auto& p : ports_){
        auto& a = p.second;
        if(a){
            a->Activate();
        }
    }
    BehaviourContainer::HandleActivate();
    logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
}

void Component::HandleConfigure(){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    for(const auto& p : ports_){
        auto& a = p.second;
        if(a){
            a->Configure();
        }
    }
    BehaviourContainer::HandleConfigure();
}

void Component::HandlePassivate(){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    for(const auto& p : ports_){
        auto& a = p.second;
        if(a){
            a->Passivate();
        }
    }

    BehaviourContainer::HandlePassivate();
    logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
}

void Component::HandleTerminate(){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);

    for(const auto& p : ports_){
        auto& port = p.second;
        if(port){
            port->Terminate();
        }
    }
    
    BehaviourContainer::HandleTerminate();
    logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::TERMINATED);
}

void Component::SetLocation(const std::vector<std::string>& location){
    component_location_ = location;
}

void Component::SetReplicationIndices(const std::vector<uint32_t>& indices){
    replication_indices_ = indices;
}

std::string Component::GetLocalisedName(){
    std::ostringstream s_stream;

    auto location_count = component_location_.size();
    auto indices_count = replication_indices_.size();

    s_stream << logger().get_experiment_name() << "/";
    if(location_count == indices_count){
        for(int i = 0; i < location_count; i++){
            s_stream << component_location_[i] << "_" << replication_indices_[i] << "/";
        }
    }
    s_stream << get_name();
    return s_stream.str();
}

const std::vector<std::string>& Component::GetLocation() const{
    return component_location_;
}

const std::vector<uint32_t>& Component::GetReplicationIndices() const{
    return replication_indices_;
}


std::weak_ptr<Port> Component::AddPort(std::unique_ptr<Port> event_port){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    if(event_port){
        const auto& port_name = event_port->get_name();
        if(ports_.count(port_name) == 0){
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
bool Component::GotCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);

    //Check for this port_name
    if(callback_type_hash_.count(port_name)){
        const auto& value = callback_type_hash_.at(port_name);
        const auto& stored_request_type = value.first.get();
        const auto& stored_reply_type = value.second.get();

        if(stored_request_type != request_type){
            std::cerr << "Stored Callback for Port '" << port_name << "' has Request Type: '" << stored_request_type.name() << "' vs Requested '" << request_type.name() << "'" << std::endl;
            return false;
        }

        if(stored_reply_type != reply_type){
            std::cerr << "Stored Callback for Port '" << port_name << "' has Reply Type: '" << stored_reply_type.name() << "' vs Requested '" << reply_type.name() << "'" << std::endl;
            return false;
        }
        return true;
    }
    return false;
}



void Component::AddCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type, std::unique_ptr<GenericCallbackWrapper> callback){
    std::lock_guard<std::mutex> ports_lock(port_mutex_);
    
    //Check for this port_name
    if(callback_type_hash_.count(port_name) || callback_functions_.count(port_name)){
        throw std::runtime_error("Component: '" + get_name() + "' Already registered Port Callback: '" + port_name + "'");
    }

    std::reference_wrapper<const std::type_info> request_type_ref = request_type;
    std::reference_wrapper<const std::type_info> reply_type_ref = reply_type;

    //Insert types into map
    callback_type_hash_.emplace(std::make_pair(port_name, std::make_pair(request_type_ref, reply_type_ref)));
    callback_functions_[port_name] = std::move(callback);
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



