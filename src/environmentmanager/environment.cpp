#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>
#include <vector>

using namespace EnvironmentManager;
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


NodeManager::ControlMessage* Environment::PopulateDeployment(NodeManager::ControlMessage& message){
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_);
    const auto& experiment_id = message.experiment_id();
    DeclusterExperiment(message);

    AddExternalPorts(experiment_id, message);


    for(const auto& node : message.nodes()){
        RecursiveAddNode(experiment_id, node);
    }

    std::string master_ip_address;
    for(const auto& attribute : message.attributes()){
        if(attribute.info().name() == "master_ip_address"){
            master_ip_address = attribute.s(0);
            break;
        }
    }
    
    SetExperimentMasterIp(experiment_id, master_ip_address);
    ConfigureNodes(experiment_id);
    auto configured_message = GetProto(experiment_id);
    FinishConfigure(experiment_id);
    return configured_message;
}

void Environment::RecursiveAddNode(const std::string& experiment_id, const NodeManager::Node& node){
    for(const auto& node : node.nodes()){
        RecursiveAddNode(experiment_id, node);
    }
    AddNodeToExperiment(experiment_id, node);
}

std::string Environment::AddDeployment(const std::string& experiment_name,
                                       const std::string& ip_address,
                                       DeploymentType deployment_type){

    switch(deployment_type){
        case DeploymentType::EXECUTION_MASTER:{
            return RegisterExperiment(experiment_name);
        }
        case DeploymentType::LOGAN_CLIENT:{
            return AddLoganClientServer();
        }
        default:
            throw std::runtime_error("Unexpected Deployment Type");
    }
}

std::string Environment::RegisterExperiment(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    if(!experiment_map_.count(experiment_name)){
        //Try and get a Manager Port, if this fails we shouldn't register
        auto manager_port = GetManagerPort();
        
        auto experiment = std::unique_ptr<EnvironmentManager::Experiment>(new EnvironmentManager::Experiment(*this, experiment_name));
        experiment_map_.emplace(experiment_name, std::move(experiment));
        experiment_map_.at(experiment_name)->SetManagerPort(manager_port);
        
        std::cout << "* Registered experiment: " << experiment_name << std::endl;
        std::cout << "* Current registered experiments: " << experiment_map_.size() << std::endl;
    }
    return experiment_map_.at(experiment_name)->GetManagerPort();
}

std::string Environment::AddLoganClientServer(){
    return GetManagerPort();
}

Experiment& Environment::GetExperiment(const std::string experiment_name){
    if(experiment_map_.count(experiment_name)){
        return *(experiment_map_.at(experiment_name));
    }
    throw std::invalid_argument("No registered experiments: '" + experiment_name + "'");
}

NodeManager::EnvironmentMessage* Environment::GetLoganDeploymentMessage(const std::string& experiment_name, const std::string& ip_address){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperiment(experiment_name);
    return experiment.GetLoganDeploymentMessage(ip_address);
}


void Environment::RemoveExperiment(const std::string& experiment_name, uint64_t time_called){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    if(experiment_map_.count(experiment_name)){
        experiment_map_.erase(experiment_name);
        std::cout << "* Current registered experiments: " << experiment_map_.size() << std::endl;
    }
}

void Environment::RemoveLoganClientServer(const std::string& experiment_name, const std::string& ip_address){

}

void Environment::FinishConfigure(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperiment(experiment_name);
    experiment.SetConfigureDone();
}

void Environment::DeclusterExperiment(NodeManager::ControlMessage& message){
    for(auto& node : *message.mutable_nodes()){
        DeclusterNode(node);
    }
}

void Environment::DeclusterNode(NodeManager::Node& node){
    //TODO: change this to have 2nd mode for distribution (reflecting components running on nodes from other experiments)
    if(node.type() == NodeManager::Node::HARDWARE_CLUSTER || node.type() == NodeManager::Node::DOCKER_CLUSTER){
        std::queue<NodeManager::Component> component_queue;
        std::queue<NodeManager::Logger> logging_servers;
        std::vector<NodeManager::Logger> logging_clients;

        for(const auto& component : node.components()){
            component_queue.emplace(component);
        }
        node.clear_components();

        for(const auto& logger : node.loggers()){
            switch(logger.type()){
                case NodeManager::Logger::SERVER:{
                    logging_servers.emplace(logger);
                    break;
                }
                case NodeManager::Logger::CLIENT:{
                    logging_clients.emplace_back(logger);
                    break;
                }
                default:
                    break;
            }
        }
        node.clear_loggers();

        int child_node_count = node.nodes_size();
        int counter = 0;

        while(!component_queue.empty()){
            //Calculate the index of the node to place each Component on
            const int node_index = counter++ % child_node_count;
            //Add a new Component and swap it with the element on the queue
            auto component = node.mutable_nodes(node_index)->add_components();
            //Copy the Component
            *component = component_queue.front();
            component_queue.pop();
        }

        counter = 0;

        //put one logging server on a node
        while(!logging_servers.empty()){
            //Calculate the index of the node to place each Component on
            const int node_index = counter++ % child_node_count;

            auto server = node.mutable_nodes(node_index)->add_loggers();
            //Copy the Server
            *server = logging_servers.front();
            logging_servers.pop();
        }

        //put a copy of all logging clients on all child nodes
        for(auto& node : *node.mutable_nodes()){
            for(const auto& logger : logging_clients){
                auto new_logger = node.add_loggers();
                //Copy the Logger
                *new_logger = logger;
            }
        }
    }

    for(auto& node : *node.mutable_nodes()){
        DeclusterNode(node);
    }
}

void Environment::AddExternalPorts(const std::string& experiment_name, const NodeManager::ControlMessage& control_message){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperiment(experiment_name);
    experiment.AddExternalPorts(control_message);
}

void Environment::AddNodeToExperiment(const std::string& experiment_name, const NodeManager::Node& node){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperiment(experiment_name);
    AddNodeToEnvironment(node);
    experiment.AddNode(node);
}

void Environment::ConfigureNodes(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperiment(experiment_name);
    experiment.ConfigureNodes();
}

NodeManager::ControlMessage* Environment::GetProto(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperiment(experiment_name);
    return experiment.GetProto();
}

std::string Environment::GetTopic(const std::string& experiment_name, const std::string& port_id){
    throw std::runtime_error("Environment::GetTopic NOT IMPLEMENTED");
}

//Return list of experiments using the topic name provided
std::vector<std::string> Environment::CheckTopic(const std::string& experiment_name, const std::string& topic){
    std::vector<std::string> out;

    std::lock_guard<std::mutex> lock(experiment_mutex_);
    for (const auto& pair : experiment_map_){
        for(const auto& check_topic : pair.second->GetTopics()){
            if(topic == check_topic){
                out.push_back(pair.first);
            }
        }
    }
    return out;
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node){
    std::string ip;
    std::string node_name = node.info().name();

    for(const auto& attribute : node.attributes()){
        if(attribute.info().name() == "ip_address"){
            ip = attribute.s(0);
            break;
        }
    }

    std::lock_guard<std::mutex> lock(node_mutex_);
    if(!node_map_.count(ip)){
        auto port_tracker = std::unique_ptr<EnvironmentManager::PortTracker>(new EnvironmentManager::PortTracker(ip, available_ports_));
        port_tracker->SetName(node_name);
        node_ip_map_.insert({node_name, ip});
        node_name_map_.insert({ip, node_name});
        node_map_.emplace(ip, std::move(port_tracker));
    }
}

//Get port from node specified.
std::string Environment::GetPort(const std::string& ip_address){
    std::lock_guard<std::mutex> lock(node_mutex_);
    if(node_map_.count(ip_address)){
        //Get first available port, store then erase it
        return node_map_.at(ip_address)->GetPort();
    }
    throw std::invalid_argument("Environment::GetPort No node found with ip address: '" + ip_address + "'");
}

//Free port specified from node specified
void Environment::FreePort(const std::string& ip_address, const std::string& port_number){
    std::lock_guard<std::mutex> lock(node_mutex_);
    if(node_map_.count(ip_address)){
        node_map_.at(ip_address)->FreePort(port_number);
    }
}

std::string Environment::GetManagerPort(){
    std::unique_lock<std::mutex> lock(port_mutex_);

    if(available_node_manager_ports_.empty()){
        throw std::invalid_argument("Environment::GetManagerPort no Available Node Manager Ports: ");
    }
    
    auto port = available_node_manager_ports_.front();
    available_node_manager_ports_.pop();
    auto port_str = std::to_string(port);
    return port_str;
}

void Environment::FreeManagerPort(const std::string& port){
    std::unique_lock<std::mutex> lock(port_mutex_);
    try{
        int port_number = std::stoi(port);
        available_node_manager_ports_.push(port_number);
    }catch(const std::invalid_argument& ex){
        std::cerr << "Trying to Free Manager Port which isn't an Int" << std::endl;
    }
}

bool Environment::NodeDeployedTo(const std::string& experiment_name, const std::string& node_ip){
     try{
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperiment(experiment_name).HasDeploymentOn(node_ip);
    }catch(const std::invalid_argument& ex){
    }
    return false;
}

bool Environment::ModelNameExists(const std::string& experiment_name){
    try{
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperiment(experiment_name).ConfigureDone();
    }catch(const std::invalid_argument& ex){
    }
    return false;
}

void Environment::SetExperimentMasterIp(const std::string& experiment_name, const std::string& ip_address){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    if(experiment_map_.count(experiment_name)){
        experiment_map_.at(experiment_name)->SetMasterIp(ip_address);
    }
}

std::string Environment::GetMasterPublisherAddress(const std::string& experiment_name){
    try{
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperiment(experiment_name).GetMasterPublisherAddress();
    }catch(const std::invalid_argument& ex){
        std::cerr << ex.what() << std::endl;
    }
    return std::string();
}

std::string Environment::GetMasterRegistrationAddress(const std::string& experiment_name){
    try{
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperiment(experiment_name).GetMasterRegistrationAddress();
    }catch(const std::invalid_argument& ex){
        std::cerr << ex.what() << std::endl;
    }
    return std::string();
}

bool Environment::ExperimentIsDirty(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_);
    return GetExperiment(experiment_name).IsDirty();
}

NodeManager::ControlMessage* Environment::GetExperimentUpdate(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_);
    return GetExperiment(experiment_name).GetUpdate();
}

//XXX: Hardcoded as This machine
std::string Environment::GetAmqpBrokerAddress(){
    return address_ + ":5672";
}

uint64_t Environment::GetClock(){
    std::lock_guard<std::mutex> lock(clock_mutex_);
    return clock_;
}

uint64_t Environment::SetClock(uint64_t incoming){
    std::lock_guard<std::mutex> lock(clock_mutex_);
    clock_ = std::max(incoming, clock_) + 1;
    return clock_;
}

uint64_t Environment::Tick(){
    std::lock_guard<std::mutex> lock(clock_mutex_);
    clock_++;
    return clock_;
}

Environment::ExternalPort& Environment::GetExternalPort(const std::string& external_port_label){
    if(external_eventport_map_.count(external_port_label)){
        return *external_eventport_map_[external_port_label];
    }
    throw std::runtime_error("Environment doesn't have an External Port with ID: '" + external_port_label + "'");
}

std::vector< std::reference_wrapper<Port> > Environment::GetExternalProducerPorts(const std::string& external_port_label){
    std::vector< std::reference_wrapper<Port> > producer_ports;
    const auto& external_port = GetExternalPort(external_port_label);

    std::vector<std::string> producer_experiments(external_port.producer_experiments.begin(), external_port.producer_experiments.end());
    //Sort the Experiment names
    std::sort(producer_experiments.begin(), producer_experiments.end());
    for(const auto& experiment_name : producer_experiments){
        auto& experiment = GetExperiment(experiment_name);
        for(auto& port : experiment.GetExternalProducerPorts(external_port_label)){
            producer_ports.emplace_back(port);
        }
    }
    return producer_ports;
}

void Environment::AddExternalConsumerPort(const std::string& experiment_name, const std::string& external_port_label){
    if(!external_eventport_map_.count(external_port_label)){
        auto external_port = new ExternalPort();
        external_port->external_label = external_port_label;
        external_eventport_map_.emplace(external_port_label, external_port);
    }
    auto& external_port = GetExternalPort(external_port_label);
    std::cerr << "* Experiment Name: " << experiment_name << " Consumes: " << external_port_label << std::endl;
    external_port.consumer_experiments.insert(experiment_name);
}

void Environment::AddExternalProducerPort(const std::string& experiment_name, const std::string& external_port_label){
    if(!external_eventport_map_.count(external_port_label)){
        auto external_port = new ExternalPort();
        external_port->external_label = external_port_label;
        external_eventport_map_.emplace(external_port_label, external_port);
    }
    auto& external_port = GetExternalPort(external_port_label);
    external_port.producer_experiments.insert(experiment_name);
    std::cerr << "* Experiment Name: " << experiment_name << " Produces: " << external_port_label << std::endl;

    //Update Consumers
    for(const auto& consumer_experiment_name : external_port.consumer_experiments){
        auto& consumer_experiment = GetExperiment(consumer_experiment_name);
        consumer_experiment.UpdatePort(external_port_label);
    }
}

void Environment::RemoveExternalConsumerPort(const std::string& experiment_name, const std::string& external_port_label){
    auto& external_port = GetExternalPort(external_port_label);
    external_port.consumer_experiments.erase(experiment_name);
    std::cerr << "* Experiment Name: " << experiment_name << " No Longer Consumes: " << external_port_label << std::endl;
}

void Environment::RemoveExternalProducerPort(const std::string& experiment_name, const std::string& external_port_label){
    auto& external_port = GetExternalPort(external_port_label);
    external_port.producer_experiments.erase(experiment_name);
    std::cerr << "* Experiment Name: " << experiment_name << " No Longer Produces: " << external_port_label << std::endl;

    //Update Consumers
    for(const auto& consumer_experiment_name : external_port.consumer_experiments){
        auto& consumer_experiment = GetExperiment(consumer_experiment_name);
        consumer_experiment.UpdatePort(external_port_label);
    }
}

const NodeManager::Attribute& Environment::GetAttributeByName(const google::protobuf::RepeatedPtrField<NodeManager::Attribute>& attribute_list, const std::string& attribute_name){
    for(const auto& attribute : attribute_list){
        if(attribute.info().name() == attribute_name){
            return attribute;
        }
    }
    throw std::invalid_argument("No attribute found with name " + attribute_name);
}
