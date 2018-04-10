#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>

Environment::Environment(int port_range_min, int port_range_max){
    PORT_RANGE_MIN = port_range_min;
    PORT_RANGE_MAX = port_range_max;

    MANAGER_PORT_RANGE_MIN = port_range_min + 10000;
    MANAGER_PORT_RANGE_MAX = port_range_max + 10000;

    //Bail out if ranges are illegal
    assert(PORT_RANGE_MIN < PORT_RANGE_MAX);
    assert(MANAGER_PORT_RANGE_MIN < MANAGER_PORT_RANGE_MAX);

    //Populate range sets
    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i <= PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }

    auto hint_iterator_manager = available_node_manager_ports_.begin();
    for(int i = MANAGER_PORT_RANGE_MIN; i < MANAGER_PORT_RANGE_MAX; i++){
        available_node_manager_ports_.insert(hint_iterator_manager, i);
        hint_iterator_manager++;
    }
    clock_ = 0;
}

std::string Environment::AddExperiment(const std::string& model_name){
    if(experiment_map_.count(model_name)){
        throw std::invalid_argument("");
    }
    experiment_map_[model_name] = new Environment::Experiment(model_name);
    auto experiment = experiment_map_[model_name];

    experiment->manager_port_ = GetManagerPort();

    return experiment->manager_port_;
}

void Environment::RemoveExperiment(const std::string& model_name, uint64_t time_called){
    //go through experiment and free all ports used.
    try{
        auto experiment = experiment_map_[model_name];
        for(const auto& port : experiment->port_map_){
            auto event_port = port.second;
            auto name = experiment->node_map_[event_port.node_id]->info().name();
            auto port_number = event_port.port_number;
            FreePort(name, port_number);
        }
        for(const auto& node : experiment->node_map_){
            auto node_struct = node.second;
            auto name = node_struct->info().name();

            if(experiment->management_port_map_.count(node.first)){
                auto management_port = experiment->management_port_map_[node.first];
                FreePort(name, management_port);

            }
            if(experiment->modellogger_port_map_.count(node.first)){
                auto modellogger_port = experiment->modellogger_port_map_[node.first];
                FreePort(name, modellogger_port);
            }
        }
        FreeManagerPort(experiment->manager_port_);
        delete experiment;
        experiment_map_.erase(model_name);
    }
    catch(...){
        std::cout << "Could not delete deployment :" << model_name << std::endl;
    }
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
    Experiment* experiment;
    if(experiment_map_.count(model_name)){
        experiment = experiment_map_[model_name];
    }
    else{
        throw std::invalid_argument("no experiment called: " + model_name);
    }

    AddNodeToEnvironment(node);

    experiment->node_map_[node.info().id()] = new NodeManager::Node(node);

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            experiment->node_address_map_[node.info().id()] = attribute.s(0);
            experiment->node_id_map_[attribute.s(0)] = node.info().id();
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
                    experiment->topic_set_.insert(event_port.topic);
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

void Environment::ConfigureNode(const std::string& model_name, NodeManager::Node& node){

    std::string node_name = node.info().name();

    //Only fill if we're actually deploying components to this node
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

        experiment_map_[model_name]->modellogger_port_map_[node.info().id()] = logger_port;
        experiment_map_[model_name]->management_port_map_[node.info().id()] = management_port;
    }
    experiment_map_[model_name]->node_map_[node.info().id()] = new NodeManager::Node(node);
}

std::vector<std::string> Environment::GetPublisherAddress(const std::string& model_name, const NodeManager::EventPort& port){
    std::vector<std::string> publisher_addresses;

    //TODO:Check that experiment exists.
    auto experiment = experiment_map_[model_name];

    //check to see if port exists in connection map. In case it does, return list of publishers
    if(port.kind() == NodeManager::EventPort::IN_PORT){

        auto publisher_port_ids = experiment->connection_map_[port.info().id()];
        //Get list of connected ports

        //Get those ports addresses
        for(auto id : publisher_port_ids){
            auto node_id = experiment->port_map_[id].node_id;
            auto port_assigned_port = experiment->port_map_[id].port_number;
            publisher_addresses.push_back("tcp://" + experiment->node_address_map_[node_id] + ":" + port_assigned_port);
        }
    }

    //In case port id does not exist in connection list, we must be a publisher. Therefore return port's deployment address.
    else if(port.kind() == NodeManager::EventPort::OUT_PORT){
        auto node_id = experiment->port_map_[port.info().id()].node_id;
        auto port_assigned_port = experiment->port_map_[port.info().id()].port_number;
        publisher_addresses.push_back("tcp://" + experiment->node_address_map_[node_id] + ":" + port_assigned_port);
    }
    return publisher_addresses;
}

std::string Environment::GetTopic(const std::string& model_name, const std::string& port_id){
    auto port = experiment_map_[model_name]->port_map_[port_id];
}

std::vector<std::string> Environment::CheckTopic(const std::string& model_name, const std::string& topic){
    std::vector<std::string> out;
    for(auto experiment_pair : experiment_map_){
        auto experiment = experiment_pair.second;
        for(auto check_topic : experiment->topic_set_){
            if(topic == check_topic){
                out.push_back(experiment->model_name_);
            }
        }
    }
    return out;
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node){
    //Bail out early yo
    if(node_map_.count(node.info().name())){
        return;
    }
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

void Environment::FreePort(const std::string& node_name, const std::string& port_number){
    auto node = node_map_[node_name];
    node->FreePort(port_number);
}

std::string Environment::GetManagerPort(){
    std::unique_lock<std::mutex> lock(port_mutex_);

    if(available_node_manager_ports_.empty()){
        return "";
    }
    auto it = available_node_manager_ports_.begin();
    auto port = *it;
    available_node_manager_ports_.erase(it);
    auto port_str =  std::to_string(port);
    return port_str;
}

void Environment::FreeManagerPort(const std::string& port){
     std::unique_lock<std::mutex> lock(port_mutex_);
    int port_number = std::stoi(port);
    available_node_manager_ports_.insert(port_number);
}

bool Environment::NodeDeployedTo(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        return experiment->node_id_map_.count(ip_address);
    }
    return false;
}

std::string Environment::GetMasterPublisherPort(const std::string& model_name, const std::string& master_ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        std::string node_name = experiment->node_map_[experiment->node_id_map_[master_ip_address]]->info().name();
        experiment->master_port_ = GetPort(node_name);
        return experiment->master_port_;
    }
    return "";
}

std::string Environment::GetNodeManagementPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        std::string node_id = experiment->node_id_map_.at(ip_address);
        return experiment->management_port_map_.at(node_id);
    }
    return "";
}
std::string Environment::GetNodeModelLoggerPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        std::string node_id = experiment->node_id_map_.at(ip_address);
        return experiment->modellogger_port_map_.at(node_id);
    }
    return "";
}

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
