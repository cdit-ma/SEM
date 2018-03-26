#include "executionmanager.h"
#include "executionparser/modelparser.h"
#include "environmentmanager/deploymentgenerator.h"
#include "environmentmanager/deploymentrule.h"
#include "environmentmanager/deploymentrules/zmq/zmqrule.h"
#include "environmentmanager/deploymentrules/dds/ddsrule.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include "controlmessage/controlmessage.pb.h"

#include <re_common/zmq/protowriter/protowriter.h>
#include "execution.hpp"

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

bool str2bool(std::string str) {
    try{
        return std::stoi(str);
    }catch(std::invalid_argument){

    }

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    int int_val = 0;
    return b;
}

void set_attr_string(NodeManager::Attribute* attr, const std::string& val){
    attr->add_s(val);
}

ExecutionManager::ExecutionManager(const std::string& endpoint, const std::string& graphml_path, double execution_duration, Execution* execution){
    if(execution){
        //Setup writer
        proto_writer_ = new zmq::ProtoWriter();
        proto_writer_->BindPublisherSocket(endpoint);

        execution_ = execution;
        execution_->AddTerminateCallback(std::bind(&ExecutionManager::TerminateExecution, this));
    }
    master_endpoint_ = endpoint;

    //Setup the parser
    auto start = std::chrono::steady_clock::now();

    protobuf_model_parser_ = new ProtobufModelParser(graphml_path);
    deployment_message_ = protobuf_model_parser_->ControlMessage();

    //TODO: parse environment manager address from commandline args.
    //TODO: parse local mode? from commandline args.
    requester_ = new EnvironmentRequester("tcp://192.168.111.230:22334", deployment_message_->model_name());

    parse_succeed_ = PopulateDeployment();
    ConstructControlMessages();


    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if(parse_succeed_){
        std::cout << "* Deployment Parsed In: " << ms.count() << " us" << std::endl;
    }else{
        std::cout << "* Deployment Parsing Failed!" << std::endl;
    }

    if(parse_succeed_ && execution){
        std::cout << "--------[Slave Registration]--------" << std::endl;
        //Start Execution thread for 60!
        execution_thread_ = new std::thread(&ExecutionManager::ExecutionLoop, this, execution_duration);
    }
}

bool ExecutionManager::PopulateDeployment(){
    if(local_mode_){
        Environment* environment = new Environment();

        environment->AddExperiment(deployment_message_->model_name());

        DeploymentGenerator generator(*environment);
        //TODO: Add other middlewares.
        generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Zmq::DeploymentRule(*environment)));
        generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Dds::DeploymentRule(*environment)));

        generator.PopulateDeployment(*deployment_message_);
    }
    else{

        requester_->Init();
        requester_->Start();
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto response = requester_->AddDeployment(*deployment_message_);

        *deployment_message_ = response;
    }
    return true;
}

void ExecutionManager::PushMessage(const std::string& topic, google::protobuf::MessageLite* message){
    proto_writer_->PushMessage(topic, message);
}

std::vector<std::string> ExecutionManager::GetSlaveAddresses(){
    std::vector<std::string> slave_addresses;

    for(const auto& ss : slave_states_){
        slave_addresses.push_back(ss.first);
    }
    return slave_addresses;
}

bool ExecutionManager::IsValid(){
    return parse_succeed_;
}

std::vector<NodeManager::ControlMessage*> ExecutionManager::getNodeStartupMessage(){
    std::vector<NodeManager::ControlMessage*> messages;
    // for(auto a : deployment_map_){
    //     messages.push_back(a.second);
    // }
    return messages;
}

bool ExecutionManager::HandleSlaveResponseMessage(const std::string& slave_address, const NodeManager::StartupResponse& response){
    auto slave_state = SlaveState::ERROR_;
    auto slave_host_name = "";//GetSlaveNameFromAddress(slave_address);
    if(response.IsInitialized()){
        slave_state = response.success() ? SlaveState::ONLINE : SlaveState::ERROR_;
    }

    if(slave_state == SlaveState::ERROR_){
        std::cerr << "* Slave: '" << slave_host_name << "' @ " << slave_address << " Error!" << std::endl;
        for(const auto& error_str : response.error_codes()){
            std::cerr << "* " << error_str << std::endl;
        }
    }else{
        std::cout << "* Slave: '" << slave_host_name << "' @ " << slave_address << " Online" << std::endl;
    }

    if(slave_states_.count(slave_address)){
        slave_states_[slave_address] = slave_state;
        
        if(GetSlaveStateCount(SlaveState::OFFLINE) == 0){
            bool should_execute = GetSlaveStateCount(SlaveState::ERROR_) == 0;
            TriggerExecution(should_execute);
        }
        return true;
    }
    return false;
}

const NodeManager::Startup ExecutionManager::GetSlaveStartupMessage(const std::string& address){
    NodeManager::Startup startup;
    startup.set_allocated_configure(new NodeManager::ControlMessage(*deployment_message_));

    auto configure = startup.configure();
    configure.clear_nodes();
    auto node = configure.add_nodes();

    *node = deployment_map_[address];

    std::string ip_address;
    std::string port;

    for(int i = 0; i < node->attributes_size(); i++){
        auto attribute = node->attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip_address = attribute.s(0);
        }
        if(attribute.info().name() == "modellogger_port"){
            port = attribute.s(0);
        }
    }

    startup.mutable_logger()->set_mode(NodeManager::Logger::CACHED);
    startup.mutable_logger()->set_publisher_address("tcp://" + ip_address + ":" + port);

    startup.set_publisher_address(master_endpoint_);
    startup.set_host_name(node->info().name());
    return startup;
}

int ExecutionManager::GetSlaveStateCount(const SlaveState& state){
    int count = 0;
    for(const auto& ss : slave_states_){
        if(ss.second == state){
            count ++;
        }
    }
    return count;
}

NodeManager::Attribute_Kind GetAttributeType(const std::string& type){
    if(type == "Integer"){
        return NodeManager::Attribute::INTEGER;
    }else if(type == "Boolean"){
        return NodeManager::Attribute::BOOLEAN;
    }else if(type == "Character"){
        return NodeManager::Attribute::CHARACTER;
    }else if(type == "String"){
        return NodeManager::Attribute::STRING;
    }else if(type == "Double"){
        return NodeManager::Attribute::DOUBLE;
    }else if(type == "Float"){
        return NodeManager::Attribute::FLOAT;
    }
    std::cerr << "Unhandle Graphml Attribute Type: '" << type << "'" << std::endl;
    return NodeManager::Attribute::STRING;
}

bool ExecutionManager::ConstructControlMessages(){
    std::unique_lock<std::mutex>(mutex_);
    std::cout << "------------[Deployment]------------" << std::endl; 

    for(int i = 0; i<deployment_message_->nodes_size(); i++){
        ConfigureNode(deployment_message_->nodes(i));
    }

    return true;
}

void ExecutionManager::ConfigureNode(const NodeManager::Node& node){

    std::string ip_address;
    std::string port;

    for(int i = 0; i<node.nodes_size(); i++){
        ConfigureNode(node.nodes(i));
    }

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip_address = attribute.s(0);
        }
        if(attribute.info().name() == "management_port"){
            port = attribute.s(0);
        }
    }
    if(node.components_size() > 0){
        std::cout << "* Slave: '" << node.info().name() << "' Deploys:" << std::endl;

        //todo: fix this
        slave_states_["tcp://" + ip_address + ":" + "7001"] = SlaveState::OFFLINE;
        
        for(int i = 0; i < node.components_size(); i++){
            std::cout << "** " << node.components(i).info().name() << " [" << node.components(i).info().type() << "]" << std::endl;
        }
    }
    //TODO: Add fail cases
    deployment_map_["tcp://" + ip_address + ":" + "7001"] = node;
}

void ExecutionManager::TriggerExecution(bool execute){
    //Obtain lock
    std::unique_lock<std::mutex> lock(execution_mutex_);
    terminate_flag_ = !execute;
    //Notify
    execution_lock_condition_.notify_all();
}

void ExecutionManager::TerminateExecution(){
    //Interupt 
    TriggerExecution(false);

    if(execution_thread_){
        //Notify
        execution_thread_->join();
    }
}

void ExecutionManager::ExecutionLoop(double duration_sec){
    auto execution_duration = std::chrono::duration<double>(duration_sec);
    
    bool execute = true;
    {
        //Wait to be executed or terminated
        std::unique_lock<std::mutex> lock(execution_mutex_);
        execution_lock_condition_.wait(lock);
        if(terminate_flag_){
            execute = false;
        }
    }

    std::cout << "-------------[Execution]------------" << std::endl;

    if(execute){
        std::cout << "* Activating Deployment" << std::endl;
        
        //Send Activate function
        auto activate = new NodeManager::ControlMessage();
        activate->set_type(NodeManager::ControlMessage::ACTIVATE);
        PushMessage("*", activate);

        {
            std::unique_lock<std::mutex> lock(execution_mutex_);
            auto cancelled = execution_lock_condition_.wait_for(lock, execution_duration, [this]{return this->terminate_flag_;});
        }

        std::cout << "* Passivating Deployment" << std::endl;
        //Send Terminate Function
        auto passivate = new NodeManager::ControlMessage();
        passivate->set_type(NodeManager::ControlMessage::PASSIVATE);
        PushMessage("*", passivate);
    }
    
    std::cout << "* Terminating Deployment" << std::endl;
    //Send Terminate Function
    auto terminate = new NodeManager::ControlMessage();
    terminate->set_type(NodeManager::ControlMessage::TERMINATE);
    PushMessage("*", terminate);

    if(!local_mode_){
        requester_->RemoveDeployment();
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    execution_->Interrupt();
}
