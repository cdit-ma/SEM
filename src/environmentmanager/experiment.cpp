#include "environment.h"
#include "experiment.h"

using namespace EnvironmentManager;

Experiment::Experiment(Environment& environment, const std::string name) : environment_(environment){
    model_name_ = name;
}

Experiment::~Experiment(){
    try{
        for(const auto& external_port_pair : external_port_map_){
            const auto& external_port_label = external_port_pair.second->external_label;
            environment_.RemoveDependentExternalExperiment(model_name_, external_port_label);
        }
        for(const auto& port : port_map_){
            if(!port.second.port_number.empty()){
                const auto& ip_address = port.second.node_ip;
                environment_.FreePort(ip_address, port.second.port_number);
            }
        }
        for(const auto& node : node_map_){
            auto node_struct = *node.second;
            auto name = node_struct.info().name();
            const auto& ip_address = node.first;

            if(management_port_map_.count(ip_address)){
                auto management_port = management_port_map_.at(ip_address);
                environment_.FreePort(ip_address, management_port);

            }
            if(modellogger_port_map_.count(ip_address)){
                auto modellogger_port = modellogger_port_map_.at(ip_address);
                environment_.FreePort(ip_address, modellogger_port);
            }
            if(orb_port_map_.count(ip_address)){
                auto orb_port = orb_port_map_.at(ip_address);
                environment_.FreePort(ip_address, orb_port);
            }
        }
        environment_.FreeManagerPort(manager_port_);
        environment_.FreePort(master_ip_address_, master_publisher_port_);
        environment_.FreePort(master_ip_address_, master_registration_port_);
    }
    catch(...){
        std::cerr << "Could not delete deployment :" << model_name_ << std::endl;
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

void Experiment::SetMasterIp(const std::string& ip){
    master_ip_address_ = ip;
}

void Experiment::AddExternalPorts(const NodeManager::ControlMessage& message){
    for(const auto& external_port : message.external_ports()){
        auto temp = new ExternalPort();
        temp->id = external_port.info().id();
        temp->external_label = external_port.info().name();
        for(const auto& connected_port_id : external_port.connected_ports()){
            temp->connected_ports.push_back(connected_port_id);
        }
        temp->is_blackbox = external_port.is_blackbox();
        external_port_map_[external_port.info().id()] = std::unique_ptr<ExternalPort>(temp);

        external_id_to_internal_id_map_[temp->external_label] = temp->id;


        //TODO: Handle this correctly!
        if(temp->is_blackbox){
            environment_.AddPublicEventPort(model_name_, temp->external_label, "");
        }
    }
}

void Experiment::AddNode(const NodeManager::Node& node){
    auto temp = std::unique_ptr<NodeManager::Node>(new NodeManager::Node(node));
    std::string ip_address;
    auto node_name = node.info().name();

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip_address = attribute.s(0);
            node_address_map_.insert({node_name, ip_address});
            break;
        }
    }
    node_map_.emplace(ip_address, std::move(temp));


    deployment_map_.insert({ip_address, 0});

    for(int i = 0; i < node.loggers_size(); i++){
        auto logger = node.loggers(i);
        if(logger.type() == NodeManager::Logger::CLIENT){

            auto logan_client = new LoganClient();
            logan_client->ip_address = ip_address;
            logan_client->id = logger.id();
            logan_client->frequency = logger.frequency();
            for(int j = 0; j < logger.processes_size(); j++){
                logan_client->processes.push_back(logger.processes(j));
            }
            logan_client->live_mode = logger.mode() == NodeManager::Logger::LIVE;
            logan_client->logging_port = environment_.GetPort(ip_address);

            logan_client_map_[logger.id()] = std::unique_ptr<LoganClient>(logan_client);
        }

        if(logger.type() == NodeManager::Logger::SERVER){
            auto logan_server = new LoganServer();

            logan_server->ip_address = ip_address;
            logan_server->db_file_name = logger.db_file_name();
            logan_server->id = logger.id();
            for(int j = 0; j < logger.client_ids_size(); j++){
                logan_server->client_ids.push_back(logger.client_ids(j));
            }

            logan_server_map_[logger.id()] = std::unique_ptr<LoganServer>(logan_server);
        }
    }

    for(int i = 0; i < node.components_size(); i++){
        auto component = node.components(i);
        for(int j = 0; j < component.ports_size(); j++){

            auto port = component.ports(j);

            EventPort event_port;
            event_port.id = port.info().id();
            event_port.guid = port.port_guid();
            event_port.node_ip = ip_address;

            if(port.middleware() == NodeManager::ZMQ){
                if(port.kind() == NodeManager::Port::PUBLISHER || port.kind() == NodeManager::Port::REPLIER){
                    event_port.port_number = environment_.GetPort(ip_address);
                }
            }
            else if(port.middleware() == NodeManager::TAO){
                if(!orb_port_map_.count(ip_address)){
                    auto orb_port = environment_.GetPort(ip_address);
                    orb_port_map_.insert({ip_address, orb_port});
                }

                event_port.port_number = orb_port_map_.at(ip_address);
                //Make a unique name
                event_port.topic = port.info().name() + "_" + event_port.id;
            }

            for(const auto& ns : port.info().namespaces()){
                event_port.type += ns + "::";
            }

            event_port.type += port.info().type();
            
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

            port_map_.insert({event_port.id, event_port});
        }
        deployment_map_.at(ip_address)++;
    }
    
    auto deploy_count = deployment_map_.at(ip_address);
    if(deploy_count > 0){
        std::cout << "Experiment[" << model_name_ << "] Node: " <<node_name << " Deployed: " << deploy_count << std::endl;
    }
}

void Experiment::ConfigureNode(NodeManager::Node& node){
    std::string node_name = node.info().name();

    std::string ip_address;

    try{
        ip_address = GetNodeIpByName(node_name);
    }catch(const std::runtime_error& ex){
        if(node.components_size() || node.loggers_size()){
            throw std::runtime_error("Deployed to unknown node: " + node_name);
        }
    }


    if(node.components_size() > 0){
        //set modellogger port
        auto logger_port = environment_.GetPort(ip_address);

        auto logger_attribute = node.add_attributes();
        auto logger_attribute_info = logger_attribute->mutable_info();
        logger_attribute->set_kind(NodeManager::Attribute::STRING);
        logger_attribute_info->set_name("modellogger_port");
        logger_attribute->add_s(logger_port);

        //set master/slave port
        auto management_port = environment_.GetPort(ip_address);

        auto management_endpoint_attribute = node.add_attributes();
        auto management_endpoint_attribute_info = management_endpoint_attribute->mutable_info();
        management_endpoint_attribute->set_kind(NodeManager::Attribute::STRING);
        management_endpoint_attribute_info->set_name("management_port");
        management_endpoint_attribute->add_s(management_port);

        modellogger_port_map_.insert({ip_address, logger_port});
        management_port_map_.insert({ip_address, management_port});
    }

    if(node.loggers_size() > 0){
        for(int i = 0; i < node.loggers_size(); i++){
            if(node.loggers(i).type() == NodeManager::Logger::SERVER){
                auto server = node.mutable_loggers(i);
                for(int j = 0; j < server->client_ids_size(); j++){
                    auto client_id = server->client_ids(j);
                    auto client_address = logan_client_map_[client_id]->ip_address;
                    auto client_port = logan_client_map_[client_id]->logging_port;
                    std::string full_address = "tcp://" + client_address + ":" + client_port;
                    server->add_client_addresses(full_address);

                    logan_server_map_[server->id()]->client_addresses.push_back(full_address);
                }
            }
        }
    }

    auto temp = std::unique_ptr<NodeManager::Node>(new NodeManager::Node(node));
    node_map_.at(ip_address).swap(temp);
}

bool Experiment::HasDeploymentOn(const std::string& ip_address) const {
    if(deployment_map_.count(ip_address)){
        return deployment_map_.at(ip_address) > 0;
    }
    else{
        throw std::invalid_argument("No node found with ip " + ip_address);
    }
}

NodeManager::EnvironmentMessage Experiment::GetLoganDeploymentMessage(const std::string& ip_address){
    NodeManager::EnvironmentMessage message;

    for(const auto& server_pair : logan_server_map_){
        auto& server = server_pair.second;
        if(server->ip_address == ip_address){
            auto logger_message = message.add_logger();
            for(const auto& client_addr : server->client_addresses){
                logger_message->add_client_addresses(client_addr);
            }
            for(const auto& model_logger_pair : modellogger_port_map_){
                logger_message->add_client_addresses("tcp://" + model_logger_pair.first + ":" + model_logger_pair.second);
            }
            logger_message->set_db_file_name(server->db_file_name);
            logger_message->set_type(NodeManager::Logger::SERVER);
        }
    }

    for(const auto& client_pair : logan_client_map_){
        auto& client = client_pair.second;
        if(client->ip_address == ip_address){
            auto logger_message = message.add_logger();
            for(const auto& process : client->processes){
                logger_message->add_processes(process);
            }
            logger_message->set_frequency(client->frequency);
            logger_message->set_publisher_address(client->ip_address);
            logger_message->set_publisher_port(client->logging_port);
            logger_message->set_type(NodeManager::Logger::CLIENT);
        }
    }

    return message;
}

std::string Experiment::GetMasterPublisherAddress(){
    if(master_publisher_port_.empty()){
        master_publisher_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_publisher_port_;
}

std::string Experiment::GetMasterRegistrationAddress(){
    if(master_registration_port_.empty()){
        master_registration_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_registration_port_;
}

std::string Experiment::GetNodeModelLoggerPort(const std::string& ip_address) const{
    return modellogger_port_map_.at(ip_address);
}

std::set<std::string> Experiment::GetTopics() const{
    return topic_set_;
}

std::vector<std::string> Experiment::GetPublisherAddress(const NodeManager::Port& port){

    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<std::string> publisher_addresses;

    if(port.kind() == NodeManager::Port::SUBSCRIBER || port.kind() == NodeManager::Port::REQUESTER){

        if(connection_map_.count(port.info().id())){
            //Get list of connected ports
            auto publisher_port_ids = connection_map_.at(port.info().id());

            //Get those ports addresses
            for(auto id : publisher_port_ids){
                auto node_ip = port_map_.at(id).node_ip;
                auto port_assigned_port = port_map_.at(id).port_number;
                publisher_addresses.push_back("tcp://" + node_ip + ":" + port_assigned_port);
            }
        }

        for(int i = 0; i < port.connected_external_ports_size(); i++){
            std::cout << "looking for " << port.connected_external_ports(i) << std::endl;
            auto external_port_label = external_port_map_.at(port.connected_external_ports(i))->external_label;
            std::cout << "looking for " << external_port_label << std::endl;
            //Check environment for public ports with this id
            if(environment_.HasPublicEventPort(external_port_label)){
                publisher_addresses.push_back(environment_.GetPublicEventPortEndpoint(external_port_label));
            }
            //We don't have this public port's address yet. The experiment it orinates from is most likely not started yet.
            //We need to keep track of the fact that this experiment is waiting for this port to become live so we can notify it of the environment change.
            else{
                environment_.AddPendingPublicEventPort(model_name_, external_port_label);
            }
        }
    }

    else if(port.kind() == NodeManager::Port::PUBLISHER || port.kind() == NodeManager::Port::REPLIER){
        auto node_ip = port_map_.at(port.info().id()).node_ip;

        auto port_assigned_port = port_map_.at(port.info().id()).port_number;
        std::string addr_string = "tcp://" + node_ip + ":" + port_assigned_port;
        publisher_addresses.push_back(addr_string);

        //Update any external port references with the address of this port.
        for(int i = 0; i < port.connected_external_ports_size(); i++){
            auto external_port_label = external_port_map_.at(port.connected_external_ports(i))->external_label;
            environment_.AddPublicEventPort(model_name_, external_port_label, addr_string);
        }
    }
    return publisher_addresses;
}

std::string Experiment::GetTaoReplierServerAddress(const NodeManager::Port& port){
    std::unique_lock<std::mutex> lock(mutex_);
    std::string server_address;

    if(port.kind() == NodeManager::Port::REQUESTER){

        //Get list of connected ports
        auto replier_port_ids = connection_map_.at(port.info().id());

        //Get those ports addresses
        for(auto id : replier_port_ids){
            auto& replier_port = port_map_.at(id);
            auto node_ip = replier_port.node_ip;
            auto port_assigned_port = replier_port.port_number;
            auto port_server_name = replier_port.topic;
            server_address = "corbaloc:iiop:" + node_ip + ":" + port_assigned_port + "/" +  port_server_name;
        }
    }

    return server_address;
}

std::string Experiment::GetTaoServerName(const NodeManager::Port& port){
    std::unique_lock<std::mutex> lock(mutex_);
    if(port.kind() == NodeManager::Port::REQUESTER || port.kind() == NodeManager::Port::REPLIER){
        //Get those ports addresses
        auto& replier_port = port_map_.at(port.info().id());
        return replier_port.topic;
    }
    return std::string();
}


std::string Experiment::GetOrbEndpoint(const std::string& port_id){
    auto node_ip = port_map_.at(port_id).node_ip;

    return node_ip + ":" + orb_port_map_.at(node_ip);
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


    for()
}

void Experiment::GetUpdate(NodeManager::ControlMessage& control_message){
    std::unique_lock<std::mutex> lock(mutex_);
    dirty_flag_ = false;

    while(!updated_port_ids_.empty()){
        auto port_it = updated_port_ids_.begin();
        auto port_external_id = *port_it;
        std::string endpoint = environment_.GetPublicEventPortEndpoint(port_external_id);
        updated_port_ids_.erase(port_it);

        std::cout << "adding " << port_external_id << std::endl;
        std::cout << "adding " << endpoint << std::endl;
        auto port_internal_id = external_id_to_internal_id_map_.at(port_external_id);
        std::cout << "adding " << port_internal_id << std::endl;

        //Iterate through all ports and check for connections to updated ports.
        //When we find one, add the new port endpoint to it's list of connecitons.
        //XXX:this is pretty terrible...
        for(int i = 0; i < deployment_message_.nodes_size(); i++){
            std::cout << i << std::endl;
            auto node = deployment_message_.nodes(i);
            for(int j = 0; j < node.components_size(); j++){
            std::cout << j << std::endl;
                auto component = node.components(j);
                for(int k = 0; k < component.ports_size(); k++){
                    auto port = component.mutable_ports(k);
            std::cout << k << std::endl;

                    for(int l = 0; l < port->connected_external_ports_size(); l++){
                        auto connected_port = port->connected_external_ports(l);
                        std::cout << connected_port << std::endl;
                        if(port_internal_id == connected_port){
                            std::cout << "yes" << std::endl;
                            for(int m = 0; m < port->attributes_size(); m++){

                                auto attribute = port->mutable_attributes(m);
                                if(attribute->info().name() == "publisher_address"){
                                    std::cout << "adding" << endpoint << " to " << port->info().name()<< std::endl;
                                    attribute->add_s(endpoint);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    control_message.CopyFrom(deployment_message_);
    std::cout << control_message.DebugString() << std::endl;
}

std::string Experiment::GetNodeIpByName(const std::string& node_name){
    try{
        return node_address_map_.at(node_name);
    }
    catch(const std::exception& ex){
        throw std::runtime_error(model_name_ + " has no registered node: '" + node_name + "'");
    }
}