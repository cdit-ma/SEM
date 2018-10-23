#include "logger.h"
#include "environment.h"
#include "experiment.h"
#include "container.h"
#include "node.h"

using namespace EnvironmentManager;

Logger::Logger(Environment& environment, Container& parent, const NodeManager::Logger& logger) :
                parent_(parent), environment_(environment){
    id_ = logger.id();

    type_ = TranslateProtoType(logger.type());

    if(type_ ==  Type::Client){
        frequency_ = logger.frequency();
        mode_ = TranslateProtoMode(logger.mode());

        for(const auto& process : logger.processes()){
            AddProcess(process);
        }

        publisher_port_ = environment_.GetPort(GetNode().GetIp());
    }else if(type_ == Type::Server){
        db_file_name_ = logger.db_file_name();

        for(const auto& client_id : logger.client_ids()){
            AddConnectedClientId(client_id);
        }
        AddConnectedClientId("model_logger");
    }
}

//Model Logger Constructor
Logger::Logger(Environment& environment, Container& parent, EnvironmentManager::Logger::Type type, EnvironmentManager::Logger::Mode mode):
                parent_(parent), environment_(environment){
    type_ = type;
    mode_ = mode;

    if(type_ == EnvironmentManager::Logger::Type::Model){
        publisher_port_ = environment_.GetPort(GetNode().GetIp());
    }
}

Logger::~Logger(){
    if(type_ == Type::Client || type_ == Type::Model){
        environment_.FreePort(GetNode().GetIp(), publisher_port_);
    }
}

std::string Logger::GetId() const{
    return id_;
}

Logger::Type Logger::GetType() const{
    return type_;
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

void Logger::AddConnectedClientId(const std::string& client_id){
    connected_client_ids_.insert(client_id);
}

const std::set<std::string>& Logger::GetConnectedClientIds(){
    return connected_client_ids_;
}

const std::set<std::string>& Logger::GetProcesses(){
    return processes_;
}

void Logger::SetDbFileName(const std::string& file_name){
    db_file_name_ = file_name;
}
std::string Logger::GetDbFileName() const{
    return db_file_name_;
}

void Logger::SetDirty(){
    if(!dirty_){
        parent_.SetDirty();
        dirty_ = true;
    }
}

bool Logger::IsDirty(){
    return dirty_;
}

std::string Logger::GetPublisherEndpoint() const{
    return "tcp://" + GetNode().GetIp() + ":" + GetPublisherPort();
}

std::unique_ptr<NodeManager::Logger> Logger::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::Logger> logger_pb;


    if(dirty_ || full_update){
        logger_pb = std::unique_ptr<NodeManager::Logger>(new NodeManager::Logger());
        switch(GetType()){
            case Type::Server:{
                //Get the clients
                for(const auto& client_id : GetConnectedClientIds()){
                    for(const auto& logger : GetExperiment().GetLoggerClients(client_id)){
                        logger_pb->add_client_addresses(logger.get().GetPublisherEndpoint());
                    }
                }

                //Fetch the model logger
                logger_pb->set_db_file_name(GetDbFileName());
                break;
            }
            case Type::Client:{
                logger_pb->set_mode(TranslateInternalMode(mode_));
                for(const auto& process : GetProcesses()){
                    logger_pb->add_processes(process);
                }
                logger_pb->set_frequency(GetFrequency());
                logger_pb->set_publisher_address(GetNode().GetIp());
                logger_pb->set_publisher_port(GetPublisherPort());
                break;
            }
            case Type::Model:{
                logger_pb->set_mode(TranslateInternalMode(mode_));
                logger_pb->set_publisher_address(GetNode().GetIp());
                logger_pb->set_publisher_port(GetPublisherPort());
                break;
            }
            default:{
                logger_pb.reset();
                break;
            }
        }

        if(logger_pb){
            //Set the Mode
            logger_pb->set_type(TranslateInternalType(type_));
        }
        
        if(dirty_){
            dirty_ = false;
        }
    }
    return logger_pb;
}

Logger::Mode Logger::TranslateProtoMode(const NodeManager::Logger::Mode mode){
    const static std::map<NodeManager::Logger::Mode, Mode> map_({
        {NodeManager::Logger::OFF, Logger::Mode::Off},
        {NodeManager::Logger::CACHED, Logger::Mode::Cached},
        {NodeManager::Logger::LIVE, Logger::Mode::Live}});
    if(map_.count(mode)){
        return map_.at(mode);
    }
    return Mode::Off;
}
NodeManager::Logger::Mode Logger::TranslateInternalMode(const Mode mode){
    const static std::map<Mode, NodeManager::Logger::Mode> map_({
        {Mode::Off, NodeManager::Logger::OFF},
        {Mode::Cached, NodeManager::Logger::CACHED},
        {Mode::Live, NodeManager::Logger::LIVE}});
    if(map_.count(mode)){
        return map_.at(mode);
    }
    return NodeManager::Logger::OFF;
}

Logger::Type Logger::TranslateProtoType(const NodeManager::Logger::Type type){
    const static std::map<NodeManager::Logger::Type, Type> map_({
        {NodeManager::Logger::MODEL, Logger::Type::Model},
        {NodeManager::Logger::CLIENT, Logger::Type::Client},
        {NodeManager::Logger::SERVER, Logger::Type::Server}});
    if(map_.count(type)){
        return map_.at(type);
    }
    return Type::None;
}
NodeManager::Logger::Type Logger::TranslateInternalType(const Type type){
    const static std::map<Type, NodeManager::Logger::Type> map_({
        {Type::None, NodeManager::Logger::NONE},
        {Type::Model, NodeManager::Logger::MODEL},
        {Type::Client, NodeManager::Logger::CLIENT},
        {Type::Server, NodeManager::Logger::SERVER}});
    if(map_.count(type)){
        return map_.at(type);
    }
    return NodeManager::Logger::NONE;
}

Node &Logger::GetNode() const {
    parent_.GetNode();
}
