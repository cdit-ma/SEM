#include "component.h"
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <future>
#include <sstream>

#include "ports/port.h"
#include "worker.h"

Component::Component(const std::string& component_name):
    BehaviourContainer(Class::COMPONENT, component_name){

}

Component::~Component(){
    Activatable::Terminate();
}

void Component::HandleActivate(){
    boost::shared_lock<boost::shared_mutex> lock{port_mutex_};
    
    for(const auto& p : ports_){
        auto& a = p.second;
        if(a){
            a->Activate();
        }
    }
    BehaviourContainer::HandleActivate();
    
}

void Component::HandleConfigure(){
    boost::shared_lock<boost::shared_mutex> lock{port_mutex_};
    
    for(const auto& p : ports_){
        auto& a = p.second;
        if(a){
            a->Configure();
        }
    }
    BehaviourContainer::HandleConfigure();
}

void Component::HandlePassivate(){
    boost::shared_lock<boost::shared_mutex> lock{port_mutex_};
    
    for(const auto& p : ports_){
        auto& a = p.second;
        if(a){
            a->Passivate();
        }
    }

    BehaviourContainer::HandlePassivate();
}

void Component::HandleTerminate(){
    boost::shared_lock<boost::shared_mutex> lock{port_mutex_};
    for(const auto& p : ports_){
        auto& port = p.second;
        if(port){
            port->Terminate();
        }
    }
    BehaviourContainer::HandleTerminate();
}

void Component::SetLocation(const std::vector<std::string>& location){
    component_location_ = location;
}

void Component::SetReplicationIndices(const std::vector<int>& indices){
    replication_indices_ = indices;
}

std::string Component::GetLocalisedName() const{
    std::ostringstream s_stream;

    auto location_count = component_location_.size();
    auto indices_count = replication_indices_.size();

    if(location_count == indices_count){
        for(int i = 0; i < location_count; i++){
            s_stream << component_location_[i] << "[" << replication_indices_[i] << "]/";
        }
    }
    s_stream << get_name();
    return s_stream.str();
}

const std::vector<std::string>& Component::GetLocation() const{
    return component_location_;
}

const std::vector<int>& Component::GetReplicationIndices() const{
    return replication_indices_;
}

std::weak_ptr<Port> Component::AddEventPort(std::unique_ptr<Port> event_port){
    if(event_port){
        boost::unique_lock<boost::shared_mutex> lock{port_mutex_};
        const auto& port_name = event_port->get_name();
        
        if(ports_.count(port_name) == 0){
            return ports_.emplace(port_name, std::move(event_port)).first->second;
        }else{
            std::cerr << "Component '" << get_name()  << "' already has an Port with name '" << port_name << "'" << std::endl;
        }
    }
    return std::weak_ptr<Port>();
}

std::weak_ptr<Port> Component::GetPort(const std::string& port_name){
    boost::shared_lock<boost::shared_mutex> lock{port_mutex_};
    auto port_itt = ports_.find(port_name);
    if(port_itt != ports_.end()){
        return port_itt->second;
    }
    return std::weak_ptr<Port>();
}

bool Component::GotCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type){
    boost::shared_lock<boost::shared_mutex> lock{port_mutex_};

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
    boost::unique_lock<boost::shared_mutex> lock{port_mutex_};
    
    //Check for this port_name
    if(callback_type_hash_.count(port_name) || callback_functions_.count(port_name)){
        throw std::runtime_error("Component: '" + get_name() + "' Already registered Port Callback: '" + port_name + "'");
    }

    std::reference_wrapper<const std::type_info> request_type_ref = request_type;
    std::reference_wrapper<const std::type_info> reply_type_ref = reply_type;

    //Insert types into map
    callback_type_hash_.emplace(std::make_pair(port_name, std::make_pair(request_type_ref, reply_type_ref)));
    callback_functions_.emplace(port_name, std::move(callback));
}



bool Component::RemoveCallback(const std::string& port_name){
    boost::unique_lock<boost::shared_mutex> lock{port_mutex_};

    auto callback_itt = callback_functions_.find(port_name);
    if(callback_itt != callback_functions_.end()){
        callback_functions_.erase(callback_itt);
        return true;
    }else{
        std::cerr << "Component '" << get_name()  << "' doesn't have a callback with name '" << port_name << "'" << std::endl;
        return false;
    }
}

const std::string& Component::GetExperimentName() const{
    return experiment_name_;
}

void Component::SetExperimentName(const std::string& experiment_name){
    if(experiment_name_.empty()){
        experiment_name_ = experiment_name;
    }else{
        logger().LogMessage(*this, "Component: " + get_name() + " has already had it's experiment name set! '" + experiment_name_ + "'");
    }
}