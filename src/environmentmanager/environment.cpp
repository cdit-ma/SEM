#include "environment.h"

#include "deploymenthandler.h"

Environment::Environment(zmq::context_t* context){
    context_ = context;
    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i < PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }
}

void Environment::AddDeployment(std::promise<std::string> port_promise, const std::string& deployment_id){
    auto deployment = new DeploymentHandler(this, context_, port_promise, deployment_id);
    deployment_map_.insert({deployment_id, deployment});
}

void Environment::RemoveDeployment(const std::string& deployment_id){

}

std::string Environment::GetDeployment(const std::string& component_id){
    return component_id;
}

std::string Environment::AddPort(const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    //Get first available port, store then erase it
    auto it = available_ports_.begin();
    int port = *it;
    available_ports_.erase(it);

    component_port_map_[component_id] = std::to_string(port);
    return std::to_string(port);
}

void Environment::RemovePort(){
    std::unique_lock<std::mutex> lock(port_mutex_);

    int port = std::stoi(component_port_map_[component_id]);
    component_port_map_.erase(component_id);

    //Return port to available port set
    available_ports_.insert(port);
}