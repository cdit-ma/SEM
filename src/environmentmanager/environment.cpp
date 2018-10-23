#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>
#include <vector>
#include <proto/controlmessage/helper.h>

using namespace EnvironmentManager;
Environment::Environment(const std::string& address, const std::string& qpid_broker_address, const std::string& tao_naming_service_address, int port_range_min, int port_range_max){
    PORT_RANGE_MIN = port_range_min;
    PORT_RANGE_MAX = port_range_max;
    qpid_broker_address_ = qpid_broker_address;
    tao_naming_service_address_ = tao_naming_service_address;

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
}

Environment::~Environment(){
    //Force that anything that requires the Environment to teardown is destroyed
    experiment_map_.clear();
}


void Environment::PopulateExperiment(const NodeManager::Experiment &const_experiment_message){
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_);
    
    //Take a copy
    auto experiment_message = const_experiment_message;
    const auto& experiment_name = experiment_message.name();

    if(experiment_map_.count(experiment_name) > 0){
        throw std::runtime_error("Got duplicate experiment name: '" + experiment_name + "'");
    }

    try{
        //Register the experiment
        RegisterExperiment(experiment_name);
        
        //Get the experiment_name
        auto& experiment = GetExperimentInternal(experiment_name);
        
        if(experiment.IsConfigured()){
            throw std::runtime_error("Experiment '" + experiment_name + "' has already been Configured once.");
        }

        //Handle Deployments etc
        DeclusterExperiment(experiment_message);

        //Handle the External Ports
        experiment.AddExternalPorts(experiment_message);
        
        //Add all nodes
        for(const auto& cluster : experiment_message.clusters()){
            AddNodes(experiment_name, cluster);
        }
        
        //Complete the Configuration
        experiment.SetConfigured();
    }catch(const std::exception& ex){
        //Remove the Experiment if we have any exceptions
        RemoveExperimentInternal(experiment_name);
        throw;
    }
}


void Environment::AddNodes(const std::string& experiment_id, const NodeManager::Cluster& cluster){
    for(const auto& node : cluster.nodes()){
        AddNodeToExperiment(experiment_id, node);
    }
}

std::string Environment::GetDeploymentHandlerPort(const std::string& experiment_name,
                                       const std::string& ip_address,
                                       DeploymentType deployment_type){

    switch(deployment_type){
        case DeploymentType::EXECUTION_MASTER:{
            return GetExperimentHandlerPort(experiment_name);
        }
        case DeploymentType::LOGAN_SERVER:{
            //Get a fresh port for the environment to service the LoganServer
            return GetManagerPort();
        }
        default:
            throw std::runtime_error("Unexpected Deployment Type");
    }
}

std::string Environment::GetExperimentHandlerPort(const std::string& experiment_name){
    return GetExperiment(experiment_name).GetManagerPort();
}


void Environment::RegisterExperiment(const std::string& experiment_name){
    if(experiment_map_.count(experiment_name)){
        throw std::runtime_error("Got duplicate experiment name: '" + experiment_name + "'");
    }
    //Try and get a Manager Port, if this fails we shouldn't register
    auto manager_port = GetManagerPort();
        
    auto experiment = std::unique_ptr<EnvironmentManager::Experiment>(new EnvironmentManager::Experiment(*this, experiment_name));
    experiment_map_.emplace(experiment_name, std::move(experiment));
    experiment_map_.at(experiment_name)->SetManagerPort(manager_port);
    std::cout << "* Registered experiment: '" << experiment_name << "'" << std::endl;
    std::cout << "* Current registered experiments: " << experiment_map_.size() << std::endl;
}

Experiment& Environment::GetExperiment(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    return GetExperimentInternal(experiment_name);
}

Experiment& Environment::GetExperimentInternal(const std::string& experiment_name){
    if(experiment_map_.count(experiment_name)){
        return *(experiment_map_.at(experiment_name));
    }
    throw std::invalid_argument("No registered experiments: '" + experiment_name + "'");
}

std::unique_ptr<NodeManager::RegisterExperimentReply> Environment::GetExperimentDeploymentInfo(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperimentInternal(experiment_name);
    return experiment.GetDeploymentInfo();
}

std::unique_ptr<NodeManager::EnvironmentMessage> Environment::GetProto(const std::string& experiment_name, const bool full_update){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperimentInternal(experiment_name);
    return experiment.GetProto(full_update);
}

void Environment::ShutdownExperiment(const std::string& experiment_name){
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_);

    auto& experiment = GetExperimentInternal(experiment_name);
    if(experiment.IsActive()){
        experiment.Shutdown();
    }else{
        RemoveExperimentInternal(experiment_name);
    }
}

void Environment::RemoveExperiment(const std::string& experiment_name){
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_);
    RemoveExperimentInternal(experiment_name);
}

void Environment::RemoveExperimentInternal(const std::string& experiment_name){
    if(experiment_map_.count(experiment_name)){
        std::cout << "* Experiment Deregistering: " << experiment_name << std::endl;
        experiment_map_.erase(experiment_name);
        std::cout << "* Current registered experiments: " << experiment_map_.size() << std::endl;
    }
}

void Environment::DeclusterExperiment(NodeManager::Experiment& message){
    for(auto& cluster : *message.mutable_clusters()){
        DeclusterCluster(cluster);
    }
}

void Environment::DeclusterCluster(NodeManager::Cluster& cluster){
    //TODO: Look into a Smart Deployment Algorithm to spread load (RE-254)
    std::queue<NodeManager::Container> container_queue;
    std::queue<NodeManager::Logger> logging_servers;
    std::vector<NodeManager::Logger> logging_clients;

    NodeManager::Container cluster_implicit_container;

    for(const auto& container : cluster.containers()){
        if(container.implicit()){
            cluster_implicit_container = container;
        }
        container_queue.emplace(container);
    }
    cluster.clear_containers();

    for(const auto& logger : cluster.loggers()){
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
    cluster.clear_loggers();

    std::queue<NodeManager::Component> component_queue;

    for(const auto& component : cluster_implicit_container.components()){
        component_queue.emplace(component);
    }

    for(const auto& logger : cluster_implicit_container.loggers()){
        switch(logger.type()){
            case NodeManager::Logger::SERVER:{
                logging_servers.emplace(logger);
                break;
            }
            case NodeManager::Logger::CLIENT:{
                logging_clients.emplace_back(logger);
                break;
            }
            default:{
                break;
            }
        }
    }

    int child_node_count = cluster.nodes_size();
    int counter = 0;

    while(!container_queue.empty()){
        //Calculate the index of the node to place each Component on
        const int node_index = counter++ % child_node_count;
        //Add a new Component and swap it with the element on the queue
        auto container = cluster.mutable_nodes(node_index)->add_containers();
        //Copy the Component
        *container = container_queue.front();
        container_queue.pop();
    }

    counter = 0;

    // Distribute components in a cluster's implicitly constructed container across node implicitly constructed containers
    while(!component_queue.empty()){
        const int node_index = counter++ % child_node_count;
        auto node = cluster.mutable_nodes(node_index);

        for(auto& container : *node->mutable_containers()){
            if(container.implicit()){
                auto component = container.add_components();
                *component = component_queue.front();
                component_queue.pop();
            }
        }
    }

    //put one logging server on a node
    while(!logging_servers.empty()){
        //Calculate the index of the node to place each Component on
        const int node_index = counter++ % child_node_count;

        auto server = cluster.mutable_nodes(node_index)->add_loggers();
        //Copy the Server
        *server = logging_servers.front();
        logging_servers.pop();
    }

    //put a copy of all logging clients on all child nodes
    for(auto& node_pb : *cluster.mutable_nodes()){
        for(const auto& logger : logging_clients){
            auto new_logger = node_pb.add_loggers();
            //Copy the Logger
            *new_logger = logger;
        }
    }
}


void Environment::AddNodeToExperiment(const std::string& experiment_name, const NodeManager::Node& node){
    auto& experiment = GetExperimentInternal(experiment_name);
    try{
        AddNodeToEnvironment(node);
        experiment.AddNode(node);
    }
    catch(const std::exception& ex){
        std::cerr << "* AddNodeToExperiment: " << ex.what() << std::endl;
        throw;
    }
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node){
    const auto& node_name = node.info().name();
    try{
        const auto& ip = node.ip_address();
        std::lock_guard<std::mutex> lock(node_mutex_);
        if(!node_map_.count(ip)){
            auto port_tracker = std::unique_ptr<EnvironmentManager::PortTracker>(new EnvironmentManager::PortTracker(ip, available_ports_));
            port_tracker->SetName(node_name);
            node_ip_map_.insert({node_name, ip});
            node_name_map_.insert({ip, node_name});
            node_map_.emplace(ip, std::move(port_tracker));
        }
    }catch(const std::exception& ex){
        //Ignore the exception
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
        return GetExperimentInternal(experiment_name).HasDeploymentOn(node_ip);
    }catch(const std::invalid_argument& ex){
    }
    return false;
}

bool Environment::IsExperimentConfigured(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    try{
        auto& experiment = GetExperimentInternal(experiment_name);
        return experiment.IsConfigured();
    }catch(const std::exception& ex){

    }
    return false;
}

bool Environment::IsExperimentRegistered(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    try{
        auto& experiment = GetExperimentInternal(experiment_name);
        return experiment.IsRegistered();
    }catch(const std::exception& ex){
    }
    return false;
}

bool Environment::IsExperimentActive(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    try{
        auto& experiment = GetExperimentInternal(experiment_name);
        return experiment.IsActive();
    }catch(const std::exception& ex){

    }
    return false;
}

void Environment::SetExperimentMasterIp(const std::string& experiment_name, const std::string& ip_address){

}

std::string Environment::GetMasterPublisherAddress(const std::string& experiment_name){
    try{
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperimentInternal(experiment_name).GetMasterPublisherAddress();
    }catch(const std::invalid_argument& ex){
        std::cerr << ex.what() << std::endl;
    }
    return std::string();
}

std::string Environment::GetMasterRegistrationAddress(const std::string& experiment_name){
    try{
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperimentInternal(experiment_name).GetMasterRegistrationAddress();
    }catch(const std::invalid_argument& ex){
        std::cerr << ex.what() << std::endl;
    }
    return std::string();
}
bool Environment::GotExperiment(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    return experiment_map_.count(experiment_name) > 0;
}

bool Environment::ExperimentIsDirty(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_);
    if(GotExperiment(experiment_name)){
        return GetExperimentInternal(experiment_name).IsDirty();
    }
    return false;
}

std::string Environment::GetAmqpBrokerAddress(){
    return qpid_broker_address_;
}

std::string Environment::GetTaoNamingServiceAddress(){
    if(tao_naming_service_address_.empty()){
        throw std::runtime_error("TAO naming service not set in environment manager.");
    }
    return tao_naming_service_address_;
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
        auto& experiment = GetExperimentInternal(experiment_name);
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
        external_eventport_map_.emplace(external_port_label, std::unique_ptr<ExternalPort>(external_port));
    }
    auto& external_port = GetExternalPort(external_port_label);
    std::cout << "* Experiment Name: '" << experiment_name << "' Consumes: '" << external_port_label << "'" << std::endl;
    external_port.consumer_experiments.insert(experiment_name);
}

void Environment::AddExternalProducerPort(const std::string& experiment_name, const std::string& external_port_label){
    if(!external_eventport_map_.count(external_port_label)){
        auto external_port = new ExternalPort();
        external_port->external_label = external_port_label;
        external_eventport_map_.emplace(external_port_label, std::unique_ptr<ExternalPort>(external_port));
    }
    auto& external_port = GetExternalPort(external_port_label);
    external_port.producer_experiments.insert(experiment_name);
    std::cout << "* Experiment Name: '" << experiment_name << "' Produces: '" << external_port_label << "'" << std::endl;

    //Update Consumers
    for(const auto& consumer_experiment_name : external_port.consumer_experiments){
        auto& consumer_experiment = GetExperimentInternal(consumer_experiment_name);
        consumer_experiment.UpdatePort(external_port_label);
    }
}

void Environment::RemoveExternalConsumerPort(const std::string& experiment_name, const std::string& external_port_label){
    auto& external_port = GetExternalPort(external_port_label);
    external_port.consumer_experiments.erase(experiment_name);
}

void Environment::RemoveExternalProducerPort(const std::string& experiment_name, const std::string& external_port_label){
    auto& external_port = GetExternalPort(external_port_label);
    external_port.producer_experiments.erase(experiment_name);
    //Update Consumers
    for(const auto& consumer_experiment_name : external_port.consumer_experiments){
        auto& consumer_experiment = GetExperimentInternal(consumer_experiment_name);
        consumer_experiment.UpdatePort(external_port_label);
    }
}

std::vector<std::string> Environment::GetExperimentNames(){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    std::vector<std::string> experiment_names;
    for(const auto& key_pair : experiment_map_){
        experiment_names.emplace_back(key_pair.first);
    }
    return experiment_names;
}