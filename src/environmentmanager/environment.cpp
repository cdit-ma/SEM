#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>
#include <vector>
#include <proto/controlmessage/helper.h>
#include <zmq/zmqutils.hpp>
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
        manager_port_range_min_(port_range_min + 10000),
        manager_port_range_max_(port_range_max + 10000)

{
    //Ensure that ports arent allocated out of 16bit port max
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

void Environment::PopulateExperiment(const NodeManager::ControlMessage& const_control_message){
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);
    
    //Take a copy
    auto control_message = const_control_message;
    const auto& experiment_name = control_message.experiment_id();

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
        DeclusterExperiment(control_message);

        //Handle the External Ports
        experiment.AddExternalPorts(control_message);
        
        //Add all nodes
        for(const auto& node : control_message.nodes()){
            RecursiveAddNode(experiment_name, node);
        }
        
        //Complete the Configuration
        experiment.SetConfigured();
    }catch(const std::exception& ex){
        //Remove the Experiment if we have any exceptions
        RemoveExperimentInternal(experiment_name);
        throw;
    }
}

void Environment::RecursiveAddNode(const std::string& experiment_id, const NodeManager::Node& node_pb){
    for(const auto& node : node_pb.nodes()){
        RecursiveAddNode(experiment_id, node);
    }
    AddNodeToExperiment(experiment_id, node_pb);
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
    std::cout << "* Registered experiment: '" << experiment_name << "'" << std::endl;
    std::cout << "* Current registered experiments: " << experiment_map_.size() << std::endl;
}

std::string Environment::AddLoganClientServer(){
    return GetManagerPort();
}

Experiment& Environment::GetExperiment(const std::string& experiment_name){
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    return GetExperimentInternal(experiment_name);
}

Experiment& Environment::GetExperimentInternal(const std::string experiment_name){
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
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);

    auto& experiment = GetExperimentInternal(experiment_name);
    if(experiment.IsActive()){
        experiment.Shutdown();
    }else{
        RemoveExperimentInternal(experiment_name);
    }
}

void Environment::RemoveExperiment(const std::string& experiment_name){
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);
    RemoveExperimentInternal(experiment_name);
}

void Environment::RemoveExperimentInternal(const std::string& experiment_name){
    if(experiment_map_.count(experiment_name)){
        std::cout << "* Experiment Deregistering: " << experiment_name << std::endl;
        experiment_map_.erase(experiment_name);
        std::cout << "* Current registered experiments: " << experiment_map_.size() << std::endl;
    }
}


void Environment::DeclusterExperiment(NodeManager::ControlMessage& message){
    for(auto& node : *message.mutable_nodes()){
        DeclusterNode(node);
    }
}

void Environment::DeclusterNode(NodeManager::Node& node){
    //TODO: Look into a Smart Deployment Algorithm to spread load (RE-254)
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
        for(auto& node_pb : *node.mutable_nodes()){
            for(const auto& logger : logging_clients){
                auto new_logger = node_pb.add_loggers();
                //Copy the Logger
                *new_logger = logger;
            }
        }
    }

    for(auto& node_pb : *node.mutable_nodes()){
        DeclusterNode(node_pb);
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
        const auto& ip = NodeManager::GetAttribute(node.attributes(), "ip_address").s(0);

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

const NodeManager::Attribute& Environment::GetAttributeByName(const google::protobuf::RepeatedPtrField<NodeManager::Attribute>& attribute_list, const std::string& attribute_name){
    for(const auto& attribute : attribute_list){
        if(attribute.info().name() == attribute_name){
            return attribute;
        }
    }
    throw std::invalid_argument("No attribute found with name " + attribute_name);
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
