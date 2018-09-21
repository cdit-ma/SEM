#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>
#include <zmq.hpp>
#include <proto/controlmessage/helper.h>

DeploymentRegister::DeploymentRegister(Execution& exe, const std::string& ip_addr, const std::string& registration_port, 
                                        const std::string& qpid_broker_address, const std::string& tao_naming_server_address,
                                        int portrange_min, int portrange_max) : execution_(exe){

    assert(portrange_min < portrange_max);
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;
    environment_ = std::unique_ptr<EnvironmentManager::Environment>(new EnvironmentManager::Environment(ip_addr, qpid_broker_address, tao_naming_server_address, portrange_min, portrange_max));

    execution_.AddTerminateCallback(std::bind(&DeploymentRegister::Terminate, this));

    replier_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
    replier_->Bind(TCPify(ip_addr_, registration_port_));
    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                  ("ExperimentRegistration", 
                                  std::bind(&DeploymentRegister::HandleAddExperiment, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                  ("NodeQuery", 
                                  std::bind(&DeploymentRegister::HandleNodeQuery, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                  ("LoganRegistration", 
                                  std::bind(&DeploymentRegister::HandleAddLoganClient, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<EnvironmentControl::ListExperimentsRequest, EnvironmentControl::ListExperimentsReply>
                                  ("ListExperiments", 
                                  std::bind(&DeploymentRegister::HandleListExperiments, this, std::placeholders::_1));
    
    replier_->RegisterProtoCallback<EnvironmentControl::ShutdownExperimentRequest, EnvironmentControl::ShutdownExperimentReply>
                                  ("ShutdownExperiment", 
                                  std::bind(&DeploymentRegister::HandleShutdownExperiment, this, std::placeholders::_1));
}

void DeploymentRegister::Start(){
    replier_->Start();
}

void DeploymentRegister::Terminate(){
    replier_->Terminate();

    deployments_.clear();
    logan_clients_.clear();
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentRegister::HandleAddExperiment(const NodeManager::EnvironmentMessage& message){

    auto reply_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage(message));

    //Push work onto new thread with port number promise
    std::promise<std::string> port_promise;
    auto port_future = port_promise.get_future();
    std::string port;

    if(environment_->GotExperiment(message.experiment_id())){
        std::cerr << "DeploymentRegister::HandleAddDeployment Exception: Tried to add preexisting experiment id." << std::endl;
        throw std::runtime_error("Tried to add preexisting experiment id.");
    }
    
    deployments_.emplace_back(new DeploymentHandler(*environment_,
                                                    ip_addr_,
                                                    EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER,
                                                    message.update_endpoint(),
                                                    std::move(port_promise),
                                                    message.experiment_id()));

    try{
        //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
        port = port_future.get();
        reply_message->set_update_endpoint(TCPify(ip_addr_, port));
        return std::move(reply_message);
    }
    catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << " (Probably out of ports)" << std::endl;
        std::string error_msg("Exception thrown by deployment register: ");
        error_msg += e.what();

        throw std::runtime_error(error_msg);
    }
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentRegister::HandleAddLoganClient(const NodeManager::EnvironmentMessage& message){
    auto reply_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage(message));

    std::string experiment_id = message.experiment_id();

    std::promise<std::string> port_promise;
    auto port_future = port_promise.get_future();
    std::string port;
    
    logan_clients_.emplace_back(new DeploymentHandler(*environment_,
                                                    ip_addr_,
                                                    EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT,
                                                    "",
                                                    std::move(port_promise),
                                                    experiment_id));

    try{
        port = port_future.get();
        reply_message->set_type(NodeManager::EnvironmentMessage::SUCCESS);
        reply_message->set_update_endpoint(TCPify(ip_addr_, port));
        return std::move(reply_message);
    }
    catch(const std::exception& ex){
        std::cerr << "Exception: " << ex.what() << " (Probably out of ports)" << std::endl;
        std::string error_msg("Exception thrown by deployment register: ");
        error_msg += ex.what();

        throw std::runtime_error(error_msg);
    }
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentRegister::HandleNodeQuery(const NodeManager::EnvironmentMessage& message){
    auto reply_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage(message));

    std::string experiment_id = message.experiment_id();

    if(message.control_message().nodes_size() < 1){
        throw std::runtime_error("No nodes supplied in node query.");
    }

    auto control_message = message.control_message();
    auto node = message.control_message().nodes(0);
    auto attrs = node.attributes();

    const auto& ip_address = NodeManager::GetAttribute(attrs, "ip_address").s(0);


    auto reply_control_message = reply_message->mutable_control_message();
    auto reply_node = reply_message->mutable_control_message()->mutable_nodes(0);
    auto reply_attrs = reply_node->mutable_attributes();

    if(ip_address.empty()){
        throw std::runtime_error("No ip_address set in message passed to HandleNodeQuery.");
    }

    if(environment_->NodeDeployedTo(experiment_id, ip_address)){
        //Have experiment_id in environment, and ip_addr has component deployed to id
        const auto& master_publisher_endpoint = environment_->GetMasterPublisherAddress(experiment_id);
        const auto& master_registration_endpoint = environment_->GetMasterRegistrationAddress(experiment_id);

        NodeManager::SetStringAttribute(reply_attrs, "master_publisher_endpoint", master_publisher_endpoint);
        NodeManager::SetStringAttribute(reply_attrs, "master_registration_endpoint", master_registration_endpoint);

        reply_message->set_type(NodeManager::EnvironmentMessage::SUCCESS);
        reply_control_message->set_type(NodeManager::ControlMessage::CONFIGURE);
    }
    else if(environment_->IsExperimentConfigured(experiment_id) == false){
        //Dont have a configured experiment with this id, send back a no_type s.t. client re-trys in a bit
        reply_message->set_type(NodeManager::EnvironmentMessage::SUCCESS);
        reply_control_message->set_type(NodeManager::ControlMessage::NO_TYPE);
    }
    else{
        //At this point, we have an experiment of the same id as ours and we aren't deployed to it.
        //Therefore terminate
        reply_message->set_type(NodeManager::EnvironmentMessage::SUCCESS);
        reply_control_message->set_type(NodeManager::ControlMessage::TERMINATE);
    }
    return std::move(reply_message);
}

std::unique_ptr<EnvironmentControl::ShutdownExperimentReply> DeploymentRegister::HandleShutdownExperiment(const EnvironmentControl::ShutdownExperimentRequest& message){
    throw std::runtime_error("NOT IMPLEMENTED");
}

std::unique_ptr<EnvironmentControl::ListExperimentsReply> DeploymentRegister::HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message){
    auto reply_message = std::unique_ptr<EnvironmentControl::ListExperimentsReply>(new EnvironmentControl::ListExperimentsReply());
    auto experiment_names = environment_->GetExperimentNames();
    experiment_names.push_back("this  is a test experiment");
    experiment_names.push_back("this  is another test experiment");
    *reply_message->mutable_experiment_names() = {experiment_names.begin(), experiment_names.end()};

    return reply_message;
}

std::string DeploymentRegister::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);

}

std::string DeploymentRegister::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}
