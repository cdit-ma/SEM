#include "environment.h"
#include "experiment.h"

using namespace EnvironmentManager;

Experiment::Experiment(Environment& environment, const std::string name) : environment_(environment){
    model_name_ = name;
}

Experiment::~Experiment(){
    try{
        for(const auto& port : port_map_){
            environment_.FreePort(port.second.node_name, port.second.port_number);
        }
        for(const auto& node : node_map_){
            auto node_struct = *node.second;
            auto name = node_struct.info().name();

            if(management_port_map_.count(node.first)){
                auto management_port = management_port_map_.at(node.first);
                environment_.FreePort(name, management_port);

            }
            if(modellogger_port_map_.count(node.first)){
                auto modellogger_port = modellogger_port_map_.at(node.first);
                environment_.FreePort(name, modellogger_port);
            }
        }
        environment_.FreeManagerPort(manager_port_);
        std::string master_node_name = node_id_map_.at(master_ip_address_);
        environment_.FreePort(master_node_name, master_port_);
    }
    catch(...){
        std::cout << "Could not delete deployment :" << model_name_ << std::endl;
    }
    std::cout << "Removed experiment: " << model_name_ << std::endl;
}

void Experiment::SetConfigureDone(){
    std::unique_lock<std::mutex> lock(mutex_);
    configure_done_ = true;
}

bool Experiment::ConfigureDone(){
    std::unique_lock<std::mutex> lock(mutex_);
    return configure_done_;
}

std::string Experiment::GetManagerPort() const{
    return manager_port_;
}

void Experiment::SetManagerPort(const std::string& manager_port){
    manager_port_ = manager_port;
}

void Experiment::SetMasterPublisherIp(const std::string& ip){
    master_ip_address_ = ip;
}

void Experiment::AddNode(const NodeManager::Node& node){
    auto temp = std::unique_ptr<NodeManager::Node>(new NodeManager::Node(node));
    auto node_name = node.info().name();
    node_map_.emplace(node_name, std::move(temp));

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            node_address_map_.insert({node_name, attribute.s(0)});
            node_id_map_.insert({attribute.s(0), node_name});
            break;
        }
    }

    deployment_map_.insert({node_name, 0});

    for(int i = 0; i < node.components_size(); i++){
        auto component = node.components(i);
        for(int j = 0; j < component.ports_size(); j++){

            auto port = component.ports(j);

            EventPort event_port;
            event_port.id = port.info().id();
            event_port.guid = port.port_guid();
            event_port.port_number = environment_.GetPort(node_name);
            event_port.node_name = node_name;

            for(const auto& ns : port.namespaces()){
                event_port.type += ns + "::";
            }
            event_port.type += port.info().type();

            std::cerr << event_port.id << " " << event_port.type << std::endl;

            for(int a = 0; a < port.attributes_size(); a++){
                auto attribute = port.attributes(a);
                if(attribute.info().name() == "topic"){
                    event_port.topic = attribute.s(0);
                    topic_set_.insert(event_port.topic);
                    break;
                }
            }

            for(int k = 0; k < port.connected_ports_size(); k++){
                auto id = port.connected_ports(k);
                connection_map_[id].push_back(event_port.id);
            }

            if(port.visibility() == NodeManager::EventPort::PUBLIC){
                environment_.AddPublicEventPort(port.port_guid(), event_port);
            }

            port_map_.insert({event_port.id, event_port});
        }
        deployment_map_.at(node_name)++;
        std::cout << node_name << " deployment count: " << deployment_map_.at(node_name) << std::endl;
    }
}

void Experiment::ConfigureNode(NodeManager::Node& node){
    std::string node_name = node.info().name();

    if(node.components_size() > 0){
        //set modellogger port
        auto logger_port = environment_.GetPort(node_name);

        auto logger_attribute = node.add_attributes();
        auto logger_attribute_info = logger_attribute->mutable_info();
        logger_attribute->set_kind(NodeManager::Attribute::STRING);
        logger_attribute_info->set_name("modellogger_port");
        logger_attribute->add_s(logger_port);

        //set master/slave port
        auto management_port = environment_.GetPort(node_name);

        auto management_endpoint_attribute = node.add_attributes();
        auto management_endpoint_attribute_info = management_endpoint_attribute->mutable_info();
        management_endpoint_attribute->set_kind(NodeManager::Attribute::STRING);
        management_endpoint_attribute_info->set_name("management_port");
        management_endpoint_attribute->add_s(management_port);

        modellogger_port_map_.insert({node_name, logger_port});
        management_port_map_.insert({node_name, management_port});
    }

    auto temp = std::unique_ptr<NodeManager::Node>(new NodeManager::Node(node));
    node_map_.at(node_name).swap(temp);
}

bool Experiment::HasDeploymentOn(const std::string& node_name) const {
    if(deployment_map_.count(node_name)){
        return deployment_map_.at(node_name) > 0;
    }
    else{
        throw std::invalid_argument("No node found with ip " + node_name);
    }
}

void Experiment::AddLoganClient(const std::string& model_name,
                                const std::string& ip_address,
                                const std::string& management_port,
                                const std::string& logging_port){
    logan_clients_.push_back(std::unique_ptr<LoganClient>(new LoganClient{model_name, ip_address, management_port, logging_port}));
}

std::string Experiment::GetMasterPublisherPort(){
    if(master_port_.empty()){
        master_port_ = environment_.GetPort(node_id_map_.at(master_ip_address_));
    }
    return master_port_;
}

std::string Experiment::GetNodeManagementPort(const std::string& node_name) const{
    return management_port_map_.at(node_name);
}

std::string Experiment::GetNodeModelLoggerPort(const std::string& node_name) const{
    return modellogger_port_map_.at(node_name);
}

std::set<std::string> Experiment::GetTopics() const{
    return topic_set_;
}

std::vector<std::string> Experiment::GetPublisherAddress(const NodeManager::EventPort& port){

    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<std::string> publisher_addresses;

    if(port.kind() == NodeManager::EventPort::IN_PORT){

        //Get list of connected ports
        auto publisher_port_ids = connection_map_.at(port.info().id());

        //Get those ports addresses
        for(auto id : publisher_port_ids){

            //XXX: Ugly hack to differentiate between local connected ports and public connected ports.
            //ID of public connected ports is set to be port guid, which is not parseable as int
            bool local_port = true;
            try{
                std::stoi(id);
            }
            catch(const std::invalid_argument& ex){
                local_port = false;
            }

            if(local_port){
                auto node_name = port_map_.at(id).node_name;
                auto port_assigned_port = port_map_.at(id).port_number;
                publisher_addresses.push_back("tcp://" + node_address_map_.at(node_name) + ":" + port_assigned_port);
            }
            else{
                //Check environment for public ports with this id
                if(environment_.HasPublicEventPort(id)){
                    publisher_addresses.push_back(environment_.GetPublicEventPortEndpoint(id));
                }
                //We don't have this public port's address yet. The experiment it orinates from is most likely not started yet.
                //We need to keep track of the fact that this experiment is waiting for this port to become live so we can notify it of the environment change.
                else{
                    environment_.AddPendingPublicEventPort(model_name_, id);
                }
            }
        }
    }

    else if(port.kind() == NodeManager::EventPort::OUT_PORT){
        auto node_name = port_map_.at(port.info().id()).node_name;
        auto port_assigned_port = port_map_.at(port.info().id()).port_number;

        std::string addr_string = "tcp://" + node_address_map_.at(node_name) + ":" + port_assigned_port;

        publisher_addresses.push_back(addr_string);

        if(port.visibility() == NodeManager::EventPort::PUBLIC){
            environment_.AddPublicEventPort(port.port_guid(), addr_string);
        }
    }
    return publisher_addresses;
}

bool Experiment::IsDirty() const{
    return dirty_flag_;
}

void Experiment::UpdatePort(const std::string& port_guid){
    dirty_flag_ = true;
    updated_port_ids_.insert(port_guid);
}

void Experiment::SetDeploymentMessage(const NodeManager::ControlMessage& control_message){
    deployment_message_ = NodeManager::ControlMessage(control_message);
}

void Experiment::GetUpdate(NodeManager::ControlMessage& control_message){
    std::unique_lock<std::mutex> lock(mutex_);
    dirty_flag_ = false;

    while(!updated_port_ids_.empty()){
        auto port_it = updated_port_ids_.begin();
        auto port_id = *port_it;
        updated_port_ids_.erase(port_it);

        //Iterate through all ports and check for connections to updated ports.
        //When we find one, add the new port endpoint to it's list of connecitons.
        //XXX: Jeez, this is pretty terrible...
        for(int i = 0; i < control_message.nodes_size(); i++){
            auto node = control_message.nodes(i);
            for(int j = 0; j < node.components_size(); j++){
                auto component = node.components(j);
                for(int k = 0; k < component.ports_size(); k++){
                    auto port = component.mutable_ports(k);
                    for(int l = 0; l < port->connected_ports_size(); l++){
                        auto connected_port = port->connected_ports(l);
                        if(port_id == connected_port){
                            try{
                                std::string endpoint = environment_.GetPublicEventPortEndpoint(port_id);
                                for(int m = 0; m < port->attributes_size(); m++){
                                    auto attribute = port->mutable_attributes(i);
                                    if(attribute->info().name() == "publisher_address"){
                                        attribute->add_s(endpoint);
                                    }
                                }
                            }
                            catch(const std::out_of_range& ex){
                                throw std::runtime_error("Updated public ports + master public ports map mismatch.");
                            }
                        }
                    }
                }
            }
        }
    }
}
