#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>
#include <vector>
#include "helper.h"
#include "node.h"
#include "zmqutils.hpp"
#include <regex>
#include <functional>
#include <google/protobuf/util/time_util.h>

using namespace EnvironmentManager;
Environment::Environment(const std::string& ip_address,
        const std::string& qpid_broker_address,
        const std::string& tao_naming_service_address,
        int port_range_min,
        int port_range_max) :
        ip_address_(ip_address),
        qpid_broker_address_(qpid_broker_address),
        tao_naming_service_address_(tao_naming_service_address),
        port_range_min_(port_range_min),
        port_range_max_(port_range_max),
        manager_port_range_min_(port_range_min + 10000),
        manager_port_range_max_(port_range_max + 10000)
{
    //Ensure that ports aren't allocated out of 16bit port max
    assert(manager_port_range_max_ < 65535);

    //Bail out if ranges are illegal
    assert(port_range_min_ < port_range_max_);
    assert(manager_port_range_min_ < manager_port_range_max_);

    //Populate range sets
    for(int i = port_range_min_; i <= port_range_max_; i++){
        available_ports_.push(i);
    }

    for(int i = manager_port_range_min_; i < manager_port_range_max_; i++){
        available_node_manager_ports_.push(i);
    }

    update_publisher_port_ = GetManagerPort();
    // TODO: Publish to update publisher when we have a new experiment or an update for an experiment.
    update_publisher_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
    update_publisher_->BindPublisherSocket(zmq::TCPify(ip_address_, update_publisher_port_));
}

Environment::~Environment(){
    //Force that anything that requires the Environment to teardown is destroyed
    experiment_map_.clear();
}


void Environment::PopulateExperiment(const NodeManager::Experiment& message){
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);

    const auto& experiment_name = message.name();
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

        //Handle the External Ports
        experiment.AddExternalPorts(message);
        
        //Add all nodes
        for(const auto& cluster : message.clusters()){
            AddNodes(experiment_name, cluster);
        }

        //Configure our master settings and check experiment validity
        experiment.ConfigureMaster();
        experiment.CheckValidity();

        //Complete the Configuration
        experiment.SetConfigured();

    }catch(const std::exception& ex){
        //Remove the Experiment if we have any exceptions
        RemoveExperimentInternal(experiment_name);
        throw;
    }
    std::cout << "* Registration complete" << std::endl;

    try {
        //Send our experiment's configuration to anything interested in updates
        update_publisher_->PushMessage(experiment_map_.at(experiment_name)->GetProto(true));
    } catch(const std::exception& ex) {
        std::cerr << "Failed to send experiment registration message to aggregation server." << std::endl;
    }
}

void Environment::AddNodes(const std::string& experiment_id, const NodeManager::Cluster& cluster){
    for(const auto& node : cluster.nodes()){
        AddNodeToExperiment(experiment_id, node);
    }

    auto& containers = cluster.containers();

    std::vector< std::reference_wrapper<const NodeManager::Container> > implicit_containers;
    
    for(const auto& container : cluster.containers()){
        if(container.implicit()){
            implicit_containers.emplace_back(container);
        }else{
            DeployContainer(experiment_id, container);
        }
    }
    for(const auto& container : implicit_containers){
        DistributeContainerToImplicitNodeContainers(experiment_id, container);
    }
}

void Environment::DistributeContainerToImplicitNodeContainers(const std::string& experiment_id, const NodeManager::Container& container){
    // will end up deployed to nodes not necessarily belonging to that cluster
    for(const auto& component : container.components()){
        GetExperimentInternal(experiment_id).GetLeastDeployedToNode().AddComponentToImplicitContainer(component);
    }

    for(const auto& logger : container.loggers()){
        if(logger.type() == NodeManager::Logger::CLIENT){
            // Add client to all nodes implicit containers.
            GetExperimentInternal(experiment_id).AddLoggingClientToNodes(logger);
        }
        if(logger.type() == NodeManager::Logger::SERVER){
            // Add server to the least deployed to node's implicit container
            GetExperimentInternal(experiment_id).GetLeastDeployedToNode(true).AddLoggingServerToImplicitContainer(logger);
        }
    }
}

void Environment::DeployContainer(const std::string& experiment_id, const NodeManager::Container& container){
    GetExperimentInternal(experiment_id).GetLeastDeployedToNode().AddContainer(container);
}

std::string Environment::GetDeploymentHandlerPort(const std::string& experiment_name,
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
    std::cout << "* Registering experiment: '" << experiment_name << "'" << std::endl;
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

    auto proto = experiment.GetProto(full_update);

    // Create copy of our control_message to send to aggregation server.
    auto update_proto_control_message = std::unique_ptr<NodeManager::EnvironmentMessage>(
            new NodeManager::EnvironmentMessage(*proto));
    update_publisher_->PushMessage(std::move(update_proto_control_message));

    return proto;
}

void Environment::ShutdownExperiment(const std::string& experiment_name){
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);
    ShutdownExperimentInternal(experiment_name);
}

std::vector<std::string> Environment::ShutdownExperimentRegex(const std::string& regex){
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);

    auto experiment_names = GetMatchingExperiments(regex);
    for(const auto& experiment_name : experiment_names){
        ShutdownExperimentInternal(experiment_name);
    }
    return experiment_names;
}

void Environment::ShutdownExperimentInternal(const std::string& experiment_name){
    auto& experiment = GetExperimentInternal(experiment_name);
    if(experiment.IsActive()){
        experiment.Shutdown();
    }else{
        RemoveExperimentInternal(experiment_name);
    }
}

std::vector<std::string> Environment::GetMatchingExperiments(const std::string& experiment_name_regex){
    std::vector<std::string> experiment_list;
    std::regex match_re(experiment_name_regex);

    for(const auto& e_pair : experiment_map_){
        std::smatch match;
        if(std::regex_match(e_pair.first, match, match_re)){
            experiment_list.emplace_back(e_pair.first);
        }
    }
    return experiment_list;
}

void Environment::RemoveExperiment(const std::string& experiment_name){
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);
    RemoveExperimentInternal(experiment_name);
}

void Environment::RemoveExperimentInternal(const std::string& experiment_name){
    if(experiment_map_.count(experiment_name)){
        std::cerr << "* Experiment Deregistering: '" << experiment_name << "'" << std::endl;
        experiment_map_.erase(experiment_name);
        std::cerr << "* Current registered experiments: " << experiment_map_.size() << std::endl;

        auto remove_experiment_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage());
        remove_experiment_message->set_type(NodeManager::EnvironmentMessage::SHUTDOWN_EXPERIMENT);
        remove_experiment_message->mutable_control_message()->set_experiment_id(experiment_name);
        using namespace google::protobuf::util;
        auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        auto timestamp = TimeUtil::MillisecondsToTimestamp(current_time.count());
        remove_experiment_message->mutable_control_message()->mutable_timestamp()->Swap(&timestamp);
        update_publisher_->PushMessage(std::move(remove_experiment_message));
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
bool Environment::GotExperiment(const std::string& experiment_name) const{
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
    if(qpid_broker_address_.empty()) {
        throw std::runtime_error("Qpid broker address not set in environment manager.");
    }
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

std::vector<std::string> Environment::GetExperimentNames() const{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    std::vector<std::string> experiment_names;
    for(const auto& key_pair : experiment_map_){
        experiment_names.emplace_back(key_pair.first);
    }
    return experiment_names;
}

std::vector<std::unique_ptr<NodeManager::ExternalPort> > Environment::GetExternalPorts() const{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    std::vector<std::unique_ptr<NodeManager::ExternalPort> > external_ports;

    for(auto& experiment_pair : experiment_map_){
        auto& experiment = experiment_pair.second;

        for(auto& external_port : experiment->GetExternalPorts()){
            auto port_message = std::unique_ptr<NodeManager::ExternalPort>(new NodeManager::ExternalPort());

            port_message->mutable_info()->set_name(external_port.get().external_label);
            port_message->mutable_info()->set_type(external_port.get().type);

            port_message->set_middleware(Port::TranslateInternalMiddleware(external_port.get().middleware));

            if(external_port.get().kind == Experiment::ExternalPort::Kind::PubSub){
                port_message->set_kind(NodeManager::ExternalPort::PUBSUB);
            }
            if(external_port.get().kind == Experiment::ExternalPort::Kind::ReqRep){
                port_message->set_kind(NodeManager::ExternalPort::SERVER);
            }
            external_ports.push_back(std::move(port_message));
        }
    }

    return external_ports;
}

std::string Environment::GetUpdatePublisherPort() const{
    return update_publisher_port_;
}
