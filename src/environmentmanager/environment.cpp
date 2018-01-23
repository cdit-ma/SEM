#include "environment.h"
#include <iostream>


Environment::Environment(){
    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i <= PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }
}

std::string Environment::AddDeployment(const std::string& deployment_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    if(available_ports_.empty()){
        return "";
    }

    //Get first available port, store then erase it
    auto it = available_ports_.begin();

    int port = *it;
    available_ports_.erase(it);

    deployment_port_map_[deployment_id] = std::to_string(port);
    return std::to_string(port);
}

void Environment::RemoveDeployment(const std::string& deployment_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    int port = std::stoi(deployment_port_map_[deployment_id]);
    deployment_port_map_.erase(deployment_id);

    //Return port to available port set
    available_ports_.insert(port);
}

//TODO: Handle rewrite of same component id during heartbeat timeout. Always use most recent.
std::string Environment::AddComponent(const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);
    if(available_ports_.empty()){
        return "";
    }

    //Get first available port, store then erase it
    auto it = available_ports_.begin();
    int port = *it;
    available_ports_.erase(it);

    component_port_map_[component_id] = std::to_string(port);
    return std::to_string(port);
}

void Environment::RemoveComponent(const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    int port = std::stoi(component_port_map_[component_id]);
    component_port_map_.erase(component_id);

    //Return port to available port set
    available_ports_.insert(port);
}
