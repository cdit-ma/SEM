#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>

Environment::Environment(int port_range_min, int port_range_max){
    PORT_RANGE_MIN = port_range_min;
    PORT_RANGE_MAX = port_range_max;
    assert(PORT_RANGE_MIN < PORT_RANGE_MAX);

    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i <= PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }
    clock_ = 0;
}

void Environment::AddExperiment(const NodeManager::ControlMessage& message){
    std::string model_name(message.model_name());
    if(experiment_map_.count(model_name)){
        throw std::invalid_argument("");
    }
    experiment_map_[model_name] = new Environment::Experiment(model_name);
}

void Environment::DeclusterExperiment(NodeManager::ControlMessage& message){
    for(int i = 0; i < message.nodes_size(); i++){
        auto node = message.mutable_nodes(i);
        DeclusterNode(*node);
    }
}

void Environment::DeclusterNode(NodeManager::Node& node){
    
    if(node.type() == NodeManager::Node::HARDWARE_CLUSTER || node.type() == NodeManager::Node::DOCKER_CLUSTER){
        std::queue<NodeManager::Component> component_queue;
        for(int i = 0; i < node.components_size(); i++){
            component_queue.push(NodeManager::Component(node.components(i)));
        }
        node.clear_components();

        int child_node_count = node.nodes_size();
        int counter = 0;

        while(!component_queue.empty()){
            auto component = component_queue.front();
            component_queue.pop();

            auto new_component = node.mutable_nodes(counter % child_node_count)->add_components();
            *new_component = component;
            counter++;
        }
    }
    for(int i = 0; i < node.nodes_size(); i++){
        DeclusterNode(*(node.mutable_nodes(i)));
    }
}

void Environment::AddNodeToExperiment(const std::string& model_name, const NodeManager::Node& node){

    AddNodeToEnvironment(node);

    experiment_map_[model_name]->node_map_[node.info().id()] = new NodeManager::Node(node);

    //TODO:Check that experiment exists.
    auto experiment = experiment_map_[model_name];

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            experiment->node_address_map_[node.info().id()] = attribute.s(0);
            break;
        }
    }
    for(int i = 0; i < node.components_size(); i++){
        auto component = node.components(i);
        for(int j = 0; j < component.ports_size(); j++){
            auto port = component.ports(j);
            
            EventPort event_port;
            event_port.id = port.info().id();
            event_port.guid = port.port_guid();
            event_port.port_number = GetPort(node.info().name());
            event_port.node_id = node.info().id();

            for(int a = 0; a < port.attributes_size(); a++){
                auto attribute = port.attributes(a);
                if(attribute.info().name() == "topic"){
                    event_port.topic = attribute.s(0);
                    break;
                }
            }

            for(int k = 0; k < port.connected_ports_size(); k++){
                auto id = port.connected_ports(k);
                experiment->connection_map_[id].push_back(event_port.id);
            }
            experiment->port_map_[event_port.id] = event_port;
        }
    }
}

void Environment::ConfigureNode(NodeManager::Node& node){

    std::string node_name = node.info().name();
    if(node.components_size() > 0){
        //set modellogger port
        auto logger_port = GetPort(node_name);
        auto logger_attribute = node.add_attributes();
        auto logger_attribute_info = logger_attribute->mutable_info();
        logger_attribute->set_kind(NodeManager::Attribute::STRING);
        logger_attribute_info->set_name("modellogger_port");
        logger_attribute->add_s(logger_port);

        //set master/slave port
        auto management_port = GetPort(node_name);
        auto management_endpoint_attribute = node.add_attributes();
        auto management_endpoint_attribute_info = management_endpoint_attribute->mutable_info();
        management_endpoint_attribute->set_kind(NodeManager::Attribute::STRING);
        management_endpoint_attribute_info->set_name("management_port");
        management_endpoint_attribute->add_s(management_port);
    }

}

std::vector<std::string> Environment::GetPublisherAddress(const std::string& model_name, const std::string& port_id){
    std::vector<std::string> publisher_addresses;

    //TODO:Check that experiment exists.
    auto experiment = experiment_map_[model_name];

    //check to see if port exists in connection map. In case it does, return list of publishers
    if(experiment->connection_map_.count(port_id)){

        auto publisher_port_ids = experiment->connection_map_[port_id];
        //Get list of connected ports

        //Get those ports addresses
        for(auto id : publisher_port_ids){
            auto node_id = experiment->port_map_[id].node_id;
            auto port_assigned_port = experiment->port_map_[id].port_number;
            publisher_addresses.push_back(experiment->node_address_map_[node_id] + ":" + port_assigned_port);
        }
    }

    //In case port id does not exist in connection list, we must be a publisher. Therefore return port's deployment address.
    else{
        auto node_id = experiment->port_map_[port_id].node_id;
        auto port_assigned_port = experiment->port_map_[port_id].port_number;
        publisher_addresses.push_back(experiment->node_address_map_[node_id] + ":" + port_assigned_port);
    }
    return publisher_addresses;
}

std::string Environment::GetTopic(const std::string& model_name, const std::string& port_id){
    auto port = experiment_map_[model_name]->port_map_[port_id];
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node){
    auto new_node = new Node(node.info().name(), available_ports_);

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            new_node->ip = attribute.s(0);
            break;
        }
    }
    node_map_[new_node->name] = new_node;

}

std::string Environment::GetPort(const std::string& node_name){
    //Get first available port, store then erase it
    auto node = node_map_[node_name];
    return node->GetPort();
    
}

std::string Environment::AddDeployment(const std::string& deployment_id, const std::string& proto_info, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);
    if(available_ports_.empty()){
        return "";
    }
    int environment_manager_port;

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

            environment_manager_port = *it;
            available_ports_.erase(it);

            deployment_info_map_[deployment_id] = new Deployment();
            deployment_info_map_[deployment_id]->id = deployment_id;
            deployment_info_map_[deployment_id]->state = DeploymentState::ACTIVE;
            deployment_info_map_[deployment_id]->time_added = time_called;
            deployment_info_map_[deployment_id]->environment_manager_port = std::to_string(environment_manager_port);
        }
    }
    else{
        //Get first available port, store then erase it
        environment_manager_port = *(available_ports_.begin());
        available_ports_.erase(available_ports_.begin());

        auto model_logger_port = *(available_ports_.begin());
        available_ports_.erase(available_ports_.begin());
        
        auto master_port = *(available_ports_.begin());
        available_ports_.erase(available_ports_.begin());

        deployment_info_map_[deployment_id] = new Deployment();
        deployment_info_map_[deployment_id]->id = deployment_id;
        deployment_info_map_[deployment_id]->state = DeploymentState::ACTIVE;
        deployment_info_map_[deployment_id]->time_added = time_called;
        deployment_info_map_[deployment_id]->environment_manager_port = std::to_string(environment_manager_port);
        deployment_info_map_[deployment_id]->master_port = std::to_string(master_port);
        deployment_info_map_[deployment_id]->model_logger_port = std::to_string(model_logger_port);
    }
    return std::to_string(environment_manager_port);
}

void Environment::RemoveDeployment(const std::string& deployment_id, uint64_t time_called){
    std::unique_lock<std::mutex> lock(port_mutex_);
    //TODO: throw exception for all lookup checks rather than find (speeds up hotpath)
    try{
        if(time_called >= deployment_info_map_.at(deployment_id)->time_added){
            int manager_port = std::stoi(deployment_info_map_.at(deployment_id)->environment_manager_port);
            int master_port = std::stoi(deployment_info_map_.at(deployment_id)->master_port);
            int model_logger_port = std::stoi(deployment_info_map_.at(deployment_id)->model_logger_port);
            deployment_info_map_.erase(deployment_id);
            //Return port to available port set
            available_ports_.insert(manager_port);
            available_ports_.insert(master_port);
            available_ports_.insert(model_logger_port);
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

std::string Environment::GetModelLoggerPort(const std::string& deployment_id){
    std::unique_lock<std::mutex> lock(port_mutex_);
    std::string out;
    try{
        out = deployment_info_map_[deployment_id]->model_logger_port;
    }
    catch(std::exception& ex){

    }
    return out;
}

std::string Environment::GetMasterPort(const std::string& deployment_id){
    std::unique_lock<std::mutex> lock(port_mutex_);
    std::string out;
    try{
        out = deployment_info_map_[deployment_id]->master_port;
    }
    catch(std::exception& ex){

    }
    return out;
}
