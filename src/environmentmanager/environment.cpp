#include "environment.h"
#include <iostream>


Environment::Environment(){
    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i <= PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }
    clock_ = 0;
}

std::string Environment::AddDeployment(const std::string& deployment_id, const std::string& proto_info, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);
    if(available_ports_.empty()){
        return "";
    }
    int port;

    if(deployment_info_map_.find(deployment_id) != deployment_info_map_.end()){
        //Already have deployment of this ID

        //If this "add deployment" call is somehow older than the currently recorded deployment, ignore
        if(time_called < deployment_info_map_[deployment_id]->time_added){
            return "";
        }

        //Update to new understanding of deployment if old version is currently timing out
        //TODO: Unsure how to handle this currently.
        //Current implementation looses refs to componenets, not good
        else if(deployment_info_map_[deployment_id]->state == DeploymentState::TIMEOUT){
            auto it = available_ports_.begin();

            port = *it;
            available_ports_.erase(it);

            deployment_info_map_[deployment_id] = new Deployment();
            deployment_info_map_[deployment_id]->id = deployment_id;
            deployment_info_map_[deployment_id]->state = DeploymentState::ACTIVE;
            deployment_info_map_[deployment_id]->time_added = time_called;

            deployment_port_map_[deployment_id] = std::to_string(port);
        }
    }
    else{
        //Get first available port, store then erase it
        auto it = available_ports_.begin();

        port = *it;
        available_ports_.erase(it);

        deployment_info_map_[deployment_id] = new Deployment();
        deployment_info_map_[deployment_id]->id = deployment_id;
        deployment_info_map_[deployment_id]->state = DeploymentState::ACTIVE;
        deployment_info_map_[deployment_id]->time_added = time_called;

        deployment_port_map_[deployment_id] = std::to_string(port);

    }
    return std::to_string(port);
}

void Environment::RemoveDeployment(const std::string& deployment_id, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);
    //TODO: throw exception for all lookup checks rather than find (speeds up hotpath)
    try{
        if(time_called >= deployment_info_map_.at(deployment_id)->time_added){
            int port = std::stoi(deployment_port_map_.at(deployment_id));
            deployment_port_map_.erase(deployment_id);

            //Return port to available port set
            available_ports_.insert(port);
        }

    }
    catch(std::out_of_range& ex){
    }
}

//TODO: Handle rewrite of same component id during heartbeat timeout. Always use most recent.
std::string Environment::AddComponent(const std::string& deployment_id, const std::string& component_id, 
                                        const std::string& proto_info, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);
    if(available_ports_.empty()){
        return "";
    }

    //Get first available port, store then erase it
    auto it = available_ports_.begin();
    int port = *it;
    available_ports_.erase(it);

    try{
        //Update our understanding of the deployment's components.
        deployment_info_map_.at(deployment_id)->component_ids.push_back(component_id);
        component_port_map_[component_id] = std::to_string(port);
    }
    catch(std::out_of_range& ex){
        std::cout << "tried to add component to non listed deployment: " << deployment_id << std::endl;
    }
    return std::to_string(port);
}

void Environment::RemoveComponent(const std::string& component_id, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);

    try{
        auto component_port = std::stoi(component_port_map_.at(component_id));
        component_port_map_.erase(component_id);
        //Return port to available port set
        available_ports_.insert(component_port);
    }
    catch(std::out_of_range& ex){
        std::cerr << "tried to remove non listed component: " << component_id << std::endl;
    }
}

uint64_t Environment::GetClock(){
    std::unique_lock<std::mutex> lock(clock_mutex_);
    return clock_;
}

uint64_t Environment::SetClock(uint64_t incoming){
    std::unique_lock<std::mutex> lock(clock_mutex_);
    clock_ = std::max(incoming, clock_) + 1;
    return clock_;
}

uint64_t Environment::Tick(){
    std::unique_lock<std::mutex> lock(clock_mutex_);
    clock_++;
    return clock_;
}

void Environment::DeploymentLive(const std::string& deployment_id, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);
    try{
        auto deployment_info = deployment_info_map_.at(deployment_id);
        if(time_called >= deployment_info->time_added){
            deployment_info->state = DeploymentState::ACTIVE;
        }
    }
    catch(std::out_of_range& ex){
        std::cerr << "Tried to live non existant deployment: " << deployment_id << std::endl;
    }
}

void Environment::DeploymentTimeout(const std::string& deployment_id, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);

    try{
        auto deployment_info = deployment_info_map_.at(deployment_id);
        if(time_called >= deployment_info->time_added){
            deployment_info->state = DeploymentState::TIMEOUT;
        }
    }
    catch(std::out_of_range& ex){
        std::cerr << "Tried to timeout non existant deployment: " << deployment_id << std::endl;
    }
}
