#include "environment.h"
#include <iostream>


Environment::Environment(){
    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i < PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }
}

std::string Environment::AddPort(const std::string& reference_port, const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    //Get first available port, store then erase it
    auto it = available_ports_.begin();
    int port = *it;
    available_ports_.erase(it);

    component_port_map_[component_id] = std::to_string(port);
    return std::to_string(port);
}

void Environment::RemovePort(const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    std::cout << component_port_map_[component_id] << std::endl;

    int port = std::stoi(component_port_map_[component_id]);
    component_port_map_.erase(component_id);

    //Return port to available port set
    available_ports_.insert(port);
}
