#include "logger.h"
#include "environment.h"
#include "experiment.h"
#include "node.h"

using namespace EnvironmentManager;

Logger::Logger(Environment& environment, Node& parent, const NodeManager::Logger& logger) : 
                node_(parent), environment_(environment){
    id_ = logger.id();

    if(logger.type() == NodeManager::Logger::CLIENT){
        type_ = EnvironmentManager::Logger::Type::Client;

        frequency_ = logger.frequency();
        for(int j = 0; j < logger.processes_size(); j++){
            processes_.insert(logger.processes(j));
        }

        if(logger.mode() == NodeManager::Logger::LIVE){
            mode_ = EnvironmentManager::Logger::Mode::Live;
        }else if(logger.mode() == NodeManager::Logger::CACHED){
            mode_ = EnvironmentManager::Logger::Mode::Cached;
        }else{
            mode_ = EnvironmentManager::Logger::Mode::Off;
        }

        publisher_port_ = environment_.GetPort(GetNode().GetIp());

        std::string addr = "tcp://" + GetNode().GetIp() + ":" + publisher_port_;
        GetExperiment().AddLoganClientEndpoint(id_, addr);
    }

    if(logger.type() == NodeManager::Logger::SERVER){
        type_ = EnvironmentManager::Logger::Type::Server;

        db_file_name_ = logger.db_file_name();
        for(int j = 0; j < logger.client_ids_size(); j++){
            client_ids_.insert(logger.client_ids(j));
        }
    }
}

Logger::Logger(Environment& environment, Node& parent, EnvironmentManager::Logger::Type type, EnvironmentManager::Logger::Mode mode): 
                node_(parent), environment_(environment){
    type_ = type;
    mode_ = mode;

    if(type_ == EnvironmentManager::Logger::Type::Model){
        publisher_port_ = environment_.GetPort(GetNode().GetIp());
        std::string endpoint = "tcp://" + GetNode().GetIp() + ":" + publisher_port_;
        GetExperiment().AddModelLoggerEndpoint(GetNode().GetId(), endpoint);
    }
}

Logger::~Logger(){
    if(type_ == Type::Client){
        environment_.FreePort(GetNode().GetIp(), publisher_port_);
        GetExperiment().RemoveLoganClientEndpoint(id_);
    }
    if(type_ == Type::Model){
        environment_.FreePort(GetNode().GetIp(), publisher_port_);
        GetExperiment().RemoveModelLoggerEndpoint(GetNode().GetId());
    }
}

void Logger::ConfigureConnections(){
    if(type_ == Type::Server){
        const auto& client_address_map = GetExperiment().GetLoganClientEndpointMap();
        for(const auto& client_id : client_ids_){
            if(client_address_map.count(client_id)){
                client_addresses_.insert(client_address_map.at(client_id));
            }
        }
        const auto& model_logger_address_map = GetExperiment().GetModelLoggerEndpointMap();
        for(const auto& address_pair : model_logger_address_map){
            client_addresses_.insert(address_pair.second);
        }
    }
}

std::string Logger::GetId() const{
    return id_;
}

Logger::Type Logger::GetType() const{
    return type_;
}

Node& Logger::GetNode() const{
    return node_;
}

Experiment& Logger::GetExperiment() const{
    return node_.GetExperiment();
}

void Logger::SetPublisherPort(const std::string& publisher_port){
    publisher_port_ = publisher_port;
}
std::string Logger::GetPublisherPort() const{
    return publisher_port_;
}

void Logger::SetFrequency(const double frequency){
    frequency_ = frequency;
}
double Logger::GetFrequency() const{
    return frequency_;
}

void Logger::SetMode(const Mode mode){
    mode_ = mode;
}
Logger::Mode Logger::GetMode() const{
    return mode_;
}

void Logger::AddProcess(const std::string& process){
    processes_.insert(process);
}
void Logger::RemoveProcess(const std::string& process){
    processes_.erase(processes_.find(process));
}
std::vector<std::string> Logger::GetProcesses() const{
    return std::vector<std::string>(processes_.begin(), processes_.end());
}

void Logger::AddClientId(const std::string& client_id){
    client_ids_.insert(client_id);
}
std::vector<std::string> Logger::GetClientIds() const{
    return std::vector<std::string>(client_ids_.begin(), client_ids_.end());
}

void Logger::AddClientAddress(const std::string& client_address){
    client_addresses_.insert(client_address);
}
std::vector<std::string> Logger::GetClientAddresses() const{
    return std::vector<std::string>(client_addresses_.begin(), client_addresses_.end());
}

void Logger::SetDbFileName(const std::string& file_name){
    db_file_name_ = file_name;
}
std::string Logger::GetDbFileName() const{
    return db_file_name_;
}

void Logger::SetDirty(){
    node_.SetDirty();
    dirty_ = true;
}
bool Logger::IsDirty(){
    return dirty_;
}

NodeManager::Logger* Logger::GetUpdate(){
    auto logger_message = new NodeManager::Logger();
    if(dirty_){

        dirty_ = false;
    }

    return logger_message;
}

NodeManager::Logger* Logger::GetDeploymentMessage() const{
    auto logger = new NodeManager::Logger();
    if(GetType() == EnvironmentManager::Logger::Type::Server){
        for(const auto& client_addr : GetClientAddresses()){
            logger->add_client_addresses(client_addr);
        }
        logger->set_db_file_name(GetDbFileName());
        logger->set_type(NodeManager::Logger::SERVER);
    }
    return logger;
}

NodeManager::Logger* Logger::GetProto(){
    NodeManager::Logger* logger;

    if(GetType() == EnvironmentManager::Logger::Type::Server){
        logger = new NodeManager::Logger();
        for(const auto& client_addr : GetClientAddresses()){
            logger->add_client_addresses(client_addr);
        }
        logger->set_db_file_name(GetDbFileName());
        logger->set_type(NodeManager::Logger::SERVER);

    }

    if(GetType() == EnvironmentManager::Logger::Type::Client){
        logger = new NodeManager::Logger();
        for(const auto& process : GetProcesses()){
            logger->add_processes(process);
        }
        logger->set_frequency(GetFrequency());
        logger->set_publisher_address(node_.GetIp());
        logger->set_publisher_port(GetPublisherPort());
        logger->set_type(NodeManager::Logger::CLIENT);
    }

    if(GetType() == EnvironmentManager::Logger::Type::Model){
        logger = new NodeManager::Logger();
        
        logger->set_publisher_address(node_.GetIp());
        logger->set_publisher_port(GetPublisherPort());
        logger->set_type(NodeManager::Logger::MODEL);
    }
    if(mode_ == Mode::Off){
        logger->set_mode(NodeManager::Logger::OFF);
    }else if(mode_ == Mode::Cached){
        logger->set_mode(NodeManager::Logger::CACHED);
    }else if(mode_ == Mode::Live){
        logger->set_mode(NodeManager::Logger::LIVE);
    }

    return logger;
}