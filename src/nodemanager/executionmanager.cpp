#include "executionmanager.h"
#include "executionparser/modelparser.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <unordered_map>

#include <re_common/proto/controlmessage/controlmessage.pb.h>
#include <re_common/zmq/protowriter/protowriter.h>
#include <re_common/util/execution.hpp>
#include <re_common/proto/controlmessage/helper.h>

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

ExecutionManager::ExecutionManager(const std::string& master_ip_addr,
                                    const std::string& graphml_path,
                                    double execution_duration,
                                    Execution* execution,
                                    const std::string& experiment_id,
                                    const std::string& environment_manager_endpoint){
    if(execution){
        //Setup writer
        proto_writer_ = new zmq::ProtoWriter();

        execution_ = execution;
        execution_->AddTerminateCallback(std::bind(&ExecutionManager::TerminateExecution, this));
    }
    

    

    master_ip_addr_ = master_ip_addr;
    experiment_id_ = experiment_id;
    environment_manager_endpoint_ = environment_manager_endpoint;

    auto start = std::chrono::steady_clock::now();
    //Setup the parser
    protobuf_model_parser_ = std::unique_ptr<ProtobufModelParser>(new ProtobufModelParser(graphml_path, experiment_id_));
    deployment_message_ = protobuf_model_parser_->ControlMessage();

    // std::cerr << deployment_message_->DebugString() << std::endl;

    NodeManager::SetStringAttribute(deployment_message_->mutable_attributes(), "master_ip_address", master_ip_addr_);

    if(!environment_manager_endpoint.empty()){
        requester_ = std::unique_ptr<EnvironmentRequester>(new EnvironmentRequester(environment_manager_endpoint, experiment_id_,
                                                            EnvironmentRequester::DeploymentType::RE_MASTER));
        requester_->AddUpdateCallback(std::bind(&ExecutionManager::UpdateCallback, this, std::placeholders::_1));
    }

    parse_succeed_ = PopulateDeployment();
    if(parse_succeed_){
        ConstructControlMessages();
        registrar_ = std::unique_ptr<zmq::Registrar>(new zmq::Registrar(*this));
    }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if(parse_succeed_){
        std::cout << "* Deployment Parsed In: " << ms.count() << " us" << std::endl;
        proto_writer_->BindPublisherSocket(master_publisher_endpoint_);

        if(parse_succeed_ && execution){
            std::cout << "--------[Slave Registration]--------" << std::endl;
            execution_future_ = std::async(std::launch::async, &ExecutionManager::ExecutionLoop, this, execution_duration);
        }
    }else{
        std::cout << "* Deployment Parsing Failed!" << std::endl;
    }
}

std::string ExecutionManager::GetMasterRegistrationEndpoint(){
    return master_registration_endpoint_;
}

bool ExecutionManager::PopulateDeployment(){
    if(local_mode_){
        EnvironmentManager::Environment* environment = new EnvironmentManager::Environment("", "", "");

        environment->AddDeployment(deployment_message_->experiment_id(), "", EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER);

        environment->PopulateExperiment(*deployment_message_);
    }
    else{
        requester_->Init(environment_manager_endpoint_);
        requester_->Start();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        NodeManager::ControlMessage response;
        try{
            //std::cout << deployment_message_->DebugString() << std::endl;
            response = requester_->AddDeployment(*deployment_message_);
        }catch(const std::runtime_error& ex){
            //If anything goes wrong, we've failed to populate our deployment. Return false
            std::cerr << "Failed to populate deployment." << std::endl;
            return false;
        }

        *deployment_message_ = response;

        //std::cout << deployment_message_->DebugString() << std::endl;
        const auto& attrs = deployment_message_->attributes();
        master_publisher_endpoint_ = NodeManager::GetAttribute(attrs, "master_publisher_endpoint").s(0);
        master_registration_endpoint_ = NodeManager::GetAttribute(attrs, "master_registration_endpoint").s(0);

        if(master_publisher_endpoint_.empty()){
            throw std::runtime_error("Got empty Master Publisher Endpoint");
        }
        if(master_registration_endpoint_.empty()){
            throw std::runtime_error("Got empty Master Registration Endpoint");
        }
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

void ExecutionManager::GotSlaveTerminated(const std::string& slave_ip){
    std::lock_guard<std::mutex> lock(slave_state_mutex_);

    if(slave_states_.count(slave_ip)){
        auto slave_hostname = GetSlaveHostName(slave_ip);
        slave_states_[slave_ip] = SlaveState::OFFLINE;
        std::cout << "* Slave: " << slave_hostname << " @ " << slave_ip << " Offline" << std::endl;
        slave_state_cv_.notify_all();
    }
}

bool ExecutionManager::HandleSlaveResponseMessage(const NodeManager::SlaveStartupResponse& response){
    auto slave_state = SlaveState::ERROR_;

    const auto& slave_ip = response.slave_ip();
    auto slave_hostname = GetSlaveHostName(slave_ip);

    if(response.IsInitialized()){
        slave_state = response.success() ? SlaveState::ONLINE : SlaveState::ERROR_;
    }

    if(slave_state == SlaveState::ERROR_){
        std::cerr << "* Slave: " << slave_hostname << " @ " << slave_ip << " Error!" << std::endl;
        for(const auto& error_str : response.error_codes()){
            std::cerr << "* [" << slave_hostname << "]: " << error_str << std::endl;
        }
    }else{
        std::cout << "* Slave: " << slave_hostname << " @ " << slave_ip << " Online" << std::endl;
    }

    {
        std::lock_guard<std::mutex> lock(slave_state_mutex_);
        if(slave_states_.count(slave_ip)){
            slave_states_[slave_ip] = slave_state;
        }
    }

    if(GetSlaveStateCount(SlaveState::OFFLINE) == 0){
        bool should_execute = GetSlaveStateCount(SlaveState::ERROR_) == 0;
        
        TriggerExecution(should_execute);
    }
    return true;
}

std::string ExecutionManager::GetSlaveHostName(const std::string& slave_ip){
    if(deployment_map_.count(slave_ip)){
        return deployment_map_[slave_ip].info().name();
    }
    return "Unknown Hostname";
}

const NodeManager::SlaveStartup ExecutionManager::GetSlaveStartupMessage(const std::string& slave_ip){
    NodeManager::SlaveStartup startup;
    startup.set_allocated_configure(new NodeManager::ControlMessage(*deployment_message_));

    auto configure = startup.mutable_configure();
    configure->clear_nodes();
    auto node = configure->add_nodes();

    if(deployment_map_.count(slave_ip)){
        *node = deployment_map_[slave_ip];
    }

    auto slave_name = GetSlaveHostName(slave_ip);
    startup.set_master_publisher_endpoint(master_publisher_endpoint_);
    startup.set_slave_host_name(slave_name);
    return startup;
}

int ExecutionManager::GetSlaveStateCount(const SlaveState& state){
    std::lock_guard<std::mutex> lock(slave_state_mutex_);
    return GetSlaveStateCountTS(state);
}

int ExecutionManager::GetSlaveStateCountTS(const SlaveState& state){
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

void ExecutionManager::ConfigureNode(const NodeManager::Node& n){
    for(const auto& node : n.nodes()){
        ConfigureNode(node);
    }

    const auto& ip_address = NodeManager::GetAttribute(node.attributes(), "ip_address").s(0);

    if(ip_address.empty()){
        throw std::runtime_error("Got no IP Address for Slave Node");
    }

    std::lock_guard<std::mutex> lock(slave_state_mutex_);
    if(node.components_size() > 0){
        std::cout << "* Slave: '" << node.info().name() << "' Deploys:" << std::endl;

        slave_states_[ip_address] = SlaveState::OFFLINE;
        
        for(int i = 0; i < node.components_size(); i++){
            std::cout << "** " << node.components(i).info().name() << " [" << node.components(i).info().type() << "]" << std::endl;
        }
    }
    //TODO: Add fail cases
    deployment_map_[ip_address] = node;
}

void ExecutionManager::UpdateCallback(NodeManager::EnvironmentMessage& environment_update){
    //TODO: filter only nodes we want to update???
    //can we even do that??

    if(environment_update.type() == NodeManager::EnvironmentMessage::UPDATE_DEPLOYMENT){
        //Take a copy.
        auto control_message = new NodeManager::ControlMessage(*environment_update.mutable_control_message());
        control_message->set_type(NodeManager::ControlMessage::CONFIGURE);
        PushMessage("*", control_message);
    }
    else{
        throw std::runtime_error("Unknown message type");
    }

}

void ExecutionManager::TriggerExecution(bool execute){
    //Obtain lock
    std::unique_lock<std::mutex> lock(execution_mutex_);
    if(execute){
        execute_flag_ = true;
    }else{
        terminate_flag_ = true;
    }

    //Notify
    execution_lock_condition_.notify_all();
}

void ExecutionManager::TerminateExecution(){
    //Interupt
    TriggerExecution(false);
    if(execution_future_.valid()){
        execution_future_.get();
    }
}

void ExecutionManager::ExecutionLoop(double duration_sec) noexcept{
    auto execution_duration = std::chrono::duration<double>(duration_sec);
    
    bool execute = false;
    {
        //Wait to be executed or terminated
        std::unique_lock<std::mutex> lock(execution_mutex_);
        execution_lock_condition_.wait(lock, [this]{return terminate_flag_ || execute_flag_;});

        if(execute_flag_){
            execute = true;
        }
    }

    std::cout << "-------------[Execution]------------" << std::endl;

    if(execute){
        //std::this_thread::sleep_for(std::chrono::seconds(0));
        std::cout << "* Activating Deployment" << std::endl;
        
        //Send Activate function
        auto activate = new NodeManager::ControlMessage();
        activate->set_type(NodeManager::ControlMessage::ACTIVATE);
        PushMessage("*", activate);

        

        {
            std::unique_lock<std::mutex> lock(execution_mutex_);
            if(duration_sec == -1){
                //Wait indefinately
                execution_lock_condition_.wait(lock, [this]{return terminate_flag_;});
            }else{
                execution_lock_condition_.wait_for(lock, execution_duration, [this]{return terminate_flag_;});
            }
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


    std::cout << "--------[Slave De-registration]--------" << std::endl;
    //Actively polling for our nodes to be dead
    while(true){
        std::unique_lock<std::mutex> lock(slave_state_mutex_);
        
        auto all_offline = slave_state_cv_.wait_for(lock, std::chrono::seconds(1), [this]{
            return GetSlaveStateCountTS(SlaveState::ONLINE) == 0;
        });

        if(all_offline){
            break;
        }
    }
    
    if(!local_mode_){
        requester_->RemoveDeployment();
    }
    execution_->Interrupt();
}
