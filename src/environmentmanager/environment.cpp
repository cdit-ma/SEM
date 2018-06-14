#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>

Environment::Environment(const std::string& address, int port_range_min, int port_range_max){
    PORT_RANGE_MIN = port_range_min;
    PORT_RANGE_MAX = port_range_max;
    address_ = address;

    MANAGER_PORT_RANGE_MIN = port_range_min + 10000;
    MANAGER_PORT_RANGE_MAX = port_range_max + 10000;

    //Ensure that ports arent allocated out of 16bit port max
    assert(MANAGER_PORT_RANGE_MAX < 65535);

    //Bail out if ranges are illegal
    assert(PORT_RANGE_MIN < PORT_RANGE_MAX);
    assert(MANAGER_PORT_RANGE_MIN < MANAGER_PORT_RANGE_MAX);

    //Populate range sets
    for(int i = PORT_RANGE_MIN; i <= PORT_RANGE_MAX; i++){
        available_ports_.push(i);
    }

    for(int i = MANAGER_PORT_RANGE_MIN; i < MANAGER_PORT_RANGE_MAX; i++){
        available_node_manager_ports_.push(i);
    }
    clock_ = 0;
}

std::string Environment::AddDeployment(const std::string& model_name,
                                        const std::string& ip_address,
                                        DeploymentType deployment_type){

    //add deployment
    if(!experiment_map_.count(model_name)){
        AddExperiment(model_name);
    }
    if(deployment_type == DeploymentType::EXECUTION_MASTER){
        return experiment_map_.at(model_name)->GetManagerPort();
    }
    if(deployment_type == DeploymentType::LOGAN_CLIENT){
        return AddLoganClient(model_name, ip_address);
    }
}

bool Environment::AddExperiment(const std::string& model_name){
    if(!experiment_map_.count(model_name)){
        auto temp = std::unique_ptr<EnvironmentManager::Experiment>(new EnvironmentManager::Experiment(*this, model_name));
        experiment_map_.emplace(model_name, std::move(temp));
        experiment_map_.at(model_name)->SetManagerPort(GetManagerPort());
    }else{
        return false;
    }
    std::cout << "Added experiment: " << model_name << std::endl;
    std::cout << "Registered experiments: " << experiment_map_.size() << std::endl;
    return true;
}

std::string Environment::AddLoganClient(const std::string& model_name, const std::string& ip_address){
    
    if(!experiment_map_.count(model_name)){
        throw std::invalid_argument("No experiment found called " + model_name);
    }

    std::string port;
    try{
        port = GetManagerPort();
    }catch(const std::exception& ex){
        port = "";
    }

    //TODO: populate this properly
    std::string logging_port = "45454";

    experiment_map_.at(model_name)->AddLoganClient(model_name, ip_address, port, logging_port);

    return port;

}

void Environment::RemoveExperiment(const std::string& model_name, uint64_t time_called){
    //go through experiment and free all ports used.
    experiment_map_.erase(model_name);
    std::cout << "Remaining experiments: " << experiment_map_.size() << std::endl;
}

void Environment::RemoveLoganClient(const std::string& model_name, const std::string& ip_address){
    //TODO: complete this.
}

void Environment::StoreControlMessage(const NodeManager::ControlMessage& control_message){
    std::string experiment_name = control_message.experiment_id();
    if(experiment_name.empty()){
        return;
    }

    if(!experiment_map_.count(experiment_name)){
        return;
    }

    experiment_map_.at(experiment_name)->SetDeploymentMessage(control_message);
    experiment_map_.at(experiment_name)->SetConfigureDone();
}

void Environment::DeclusterExperiment(NodeManager::ControlMessage& message){
    for(int i = 0; i < message.nodes_size(); i++){
        auto node = message.mutable_nodes(i);
        DeclusterNode(*node);
    }
}

void Environment::DeclusterNode(NodeManager::Node& node){
    //TODO: change this to have 2nd mode for distribution (reflecting components running on nodes from other experiments)
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
    if(experiment_map_.count(model_name)){
        AddNodeToEnvironment(node);
        experiment_map_.at(model_name)->AddNode(node);
    }
    else{
        throw std::invalid_argument("No experiment called: " + model_name);
    }

}

void Environment::ConfigureNode(const std::string& model_name, NodeManager::Node& node){
    if(experiment_map_.count(model_name)){
        experiment_map_.at(model_name)->ConfigureNode(node);
    }
    else{
        throw std::invalid_argument("No experiment called: " + model_name);
    }
}

std::vector<std::string> Environment::GetPublisherAddress(const std::string& model_name, const NodeManager::Port& port){
    if(experiment_map_.count(model_name)){
        return experiment_map_.at(model_name)->GetPublisherAddress(port);
    }else{
        throw std::invalid_argument("Model named :" + model_name + " not found.");
    }
}

std::string Environment::GetTopic(const std::string& model_name, const std::string& port_id){
    //auto port = experiment_map_[model_name]->port_map_[port_id];

    throw std::runtime_error("Environment::GetTopic NOT IMPLEMENTED");
    return "";

}

std::string Environment::GetOrbEndpoint(const std::string& model_name, const std::string& port_id){
    if(experiment_map_.count(model_name)){
        return experiment_map_.at(model_name)->GetOrbEndpoint(port_id);
    }
}

//Return list of experiments using the topic name provided
std::vector<std::string> Environment::CheckTopic(const std::string& model_name, const std::string& topic){
    std::vector<std::string> out;
    for(auto&& experiment_pair : experiment_map_){
        for(auto check_topic : experiment_pair.second->GetTopics()){
            if(topic == check_topic){
                out.push_back(experiment_pair.first);
            }
        }
    }
    return out;
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node){
    std::string ip;
    std::string node_name = node.info().name();
    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip = attribute.s(0);
            break;
        }
    }

    if(node_map_.count(node_name)){
        return;
    }
    auto temp = std::unique_ptr<EnvironmentManager::Node>(new EnvironmentManager::Node(node.info().name(), available_ports_));
    temp->SetName(node_name);
    node_ip_map_.insert(std::make_pair(node_name, ip));
    node_name_map_.insert(std::make_pair(ip, node_name));
    node_map_.emplace(node_name, std::move(temp));
}

//Get port from node specified.
std::string Environment::GetPort(const std::string& node_name){
    //Get first available port, store then erase it
    if(node_map_.count(node_name)){
        std::cout << "Get port " << node_name << std::endl;
        return node_map_.at(node_name)->GetPort();
    }
    else{
        throw std::invalid_argument("Environment::GetPort No node found with name: " + node_name);
    }
}

//Free port specified from node specified
void Environment::FreePort(const std::string& node_name, const std::string& port_number){
    if(node_map_.count(node_name)){
        std::cout << "Free port " << node_name << ":" << port_number << std::endl;
        node_map_.at(node_name)->FreePort(port_number);
    }
}

std::string Environment::GetManagerPort(){
    std::unique_lock<std::mutex> lock(port_mutex_);

    if(available_node_manager_ports_.empty()){
        return "";
    }
    auto port = available_node_manager_ports_.front();
    available_node_manager_ports_.pop();
    auto port_str = std::to_string(port);
    return port_str;
}

void Environment::FreeManagerPort(const std::string& port){
    std::unique_lock<std::mutex> lock(port_mutex_);
    int port_number = std::stoi(port);
    available_node_manager_ports_.push(port_number);
}

bool Environment::NodeDeployedTo(const std::string& model_name, const std::string& node_ip) const{
    if(experiment_map_.count(model_name)){
        std::string node_name;
        try{
            node_name = node_name_map_.at(node_ip);
        }
        catch(const std::exception& ex){
        }
        return experiment_map_.at(model_name)->HasDeploymentOn(node_name);
    }
    return false;
}

bool Environment::ModelNameExists(const std::string& model_name) const{
    return experiment_map_.count(model_name);
}

std::string Environment::GetMasterPublisherPort(const std::string& model_name, const std::string& master_ip_address){
    if(experiment_map_.count(model_name)){
        try{
            experiment_map_.at(model_name)->SetMasterPublisherIp(master_ip_address);
            return experiment_map_.at(model_name)->GetMasterPublisherPort();
        }catch(std::runtime_error ex){
            
            return "";
        }
    }
    return "";
}

std::string Environment::GetNodeManagementPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        return experiment_map_.at(model_name)->GetNodeManagementPort(node_name_map_.at(ip_address));
    }
    return "";
}
std::string Environment::GetNodeModelLoggerPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        return experiment_map_.at(model_name)->GetNodeModelLoggerPort(node_name_map_.at(ip_address));
    }
    return "";
}

std::string Environment::GetLoganPublisherPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        return experiment_map_.at(model_name)->GetNodeModelLoggerPort(ip_address);
    }
    return "";
}

std::vector<std::string> Environment::GetLoganClientList(const std::string& model_name){

    if(experiment_map_.count(model_name)){
        return experiment_map_.at(model_name)->GetLoganClientList();
    }

    return {};
}

bool Environment::ExperimentIsDirty(const std::string& model_name){
    try{
        return experiment_map_.at(model_name)->IsDirty();
    }catch(const std::out_of_range& ex){
        throw std::invalid_argument("Model named " + model_name + " not found in Environment::ExperimentIsDirty.");
    }
}

void Environment::GetExperimentUpdate(const std::string& model_name, NodeManager::ControlMessage& control_message){
    try{
        experiment_map_.at(model_name)->GetUpdate(control_message);
    }catch(const std::out_of_range& ex){
        throw std::invalid_argument("Model named " + model_name + " not found in Environment::GetExperimentUpdate.");
    }
}

bool Environment::HasPublicEventPort(const std::string& port_id){
    return public_event_port_map_.count(port_id);
}

std::string Environment::GetPublicEventPortEndpoint(const std::string& port_id){
    if(public_event_port_map_.count(port_id)){
        return public_event_port_map_.at(port_id)->endpoint;
    }
    else{
        throw std::invalid_argument("Endpoint with id: " + port_id + " not found!");
    }
}

void Environment::AddPublicEventPort(const std::string& port_guid, const std::string& address){

    auto temp = std::unique_ptr<EnvironmentManager::EventPort>(new EnvironmentManager::EventPort());
    public_event_port_map_.emplace(port_guid, std::move(temp));
    public_event_port_map_.at(port_guid)->id = port_guid;
    public_event_port_map_.at(port_guid)->guid = port_guid;
    public_event_port_map_.at(port_guid)->endpoint = address;

    if(pending_port_map_.count(port_guid)){
        for(auto experiment_id : pending_port_map_.at(port_guid)){
            experiment_map_.at(experiment_id)->UpdatePort(port_guid);
        }
        pending_port_map_.erase(port_guid);
    }
}

void Environment::AddPublicEventPort(const std::string& port_guid, EnvironmentManager::EventPort event_port){
    auto temp = std::unique_ptr<EnvironmentManager::EventPort>(new EnvironmentManager::EventPort(event_port));
    
    public_event_port_map_.emplace(port_guid, std::move(temp));

    if(pending_port_map_.count(port_guid)){
        for(auto experiment_id : pending_port_map_.at(port_guid)){
            experiment_map_.at(experiment_id)->UpdatePort(port_guid);
        }
        pending_port_map_.erase(port_guid);
    }
}

bool Environment::HasPendingPublicEventPort(const std::string& port_id){
    return pending_port_map_.count(port_id);
}

std::set<std::string> Environment::GetDependentExperiments(const std::string& port_id){
    return pending_port_map_.at(port_id);
}

void Environment::AddPendingPublicEventPort(const std::string& model_name, const std::string& port_id){
    pending_port_map_.at(port_id).insert(model_name);
}

//XXX: Hardcoded
std::string Environment::GetAmqpBrokerAddress(){
    return address_ + ":5672";
}

/*
void Environment::ExperimentLive(const std::string& model_name, uint64_t time_called){
    try{
        auto experiment = experiment_map_.at(model_name);
        if(time_called >= experiment->time_added){
            experiment->state = ExperimentState::ACTIVE;
        }
    }
    catch(std::out_of_range& ex){
        std::cerr << "Tried to live non existant experiment: " << model_name << std::endl;
    }
}

void Environment::ExperimentTimeout(const std::string& model_name, uint64_t time_called){
    try{
        auto experiment = experiment_map_.at(model_name);
        if(time_called >= experiment->time_added){
            experiment->state = ExperimentState::TIMEOUT;
        }
    }
    catch(std::out_of_range& ex){
        std::cerr << "Tried to timeout non existant deployment: " << model_name << std::endl;
    }
}
*/
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
