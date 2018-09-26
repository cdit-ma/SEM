#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>
#include <zmq.hpp>
#include <zmq/zmqutils.hpp>
#include <proto/controlmessage/helper.h>
#include "node.h"

DeploymentRegister::DeploymentRegister(Execution& exe, const std::string& environment_manager_ip_address, const std::string& registration_port, 
                                        const std::string& qpid_broker_address, const std::string& tao_naming_server_address,
                                        int portrange_min, int portrange_max)
                                        :
execution_(exe),
environment_manager_ip_address_(environment_manager_ip_address)
{

    assert(portrange_min < portrange_max);
    environment_ = std::unique_ptr<EnvironmentManager::Environment>(new EnvironmentManager::Environment(environment_manager_ip_address, qpid_broker_address, tao_naming_server_address, portrange_min, portrange_max));

    execution_.AddTerminateCallback(std::bind(&DeploymentRegister::Terminate, this));

    replier_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
    replier_->Bind(zmq::TCPify(environment_manager_ip_address, registration_port));


    //Bind Re Node Manager Functions
    replier_->RegisterProtoCallback<NodeManager::NodeManagerRegistrationRequest, NodeManager::NodeManagerRegistrationReply>
                                  ("NodeManagerRegistration", 
                                  std::bind(&DeploymentRegister::HandleNodeManagerRegistration, this, std::placeholders::_1));

    //Logan Managed Server Functions
    replier_->RegisterProtoCallback<NodeManager::LoganRegistrationRequest, NodeManager::LoganRegistrationReply>
                                  ("LoganRegistration", 
                                  std::bind(&DeploymentRegister::HandleLoganRegistration, this, std::placeholders::_1));

    //Controller Functions
    replier_->RegisterProtoCallback<NodeManager::RegisterExperimentRequest, NodeManager::RegisterExperimentReply>
                                  ("RegisterExperiment", 
                                  std::bind(&DeploymentRegister::HandleRegisterExperiment, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<EnvironmentControl::ShutdownExperimentRequest, EnvironmentControl::ShutdownExperimentReply>
                                  ("ShutdownExperiment", 
                                  std::bind(&DeploymentRegister::HandleShutdownExperiment, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<EnvironmentControl::ListExperimentsRequest, EnvironmentControl::ListExperimentsReply>
                                  ("ListExperiments", 
                                  std::bind(&DeploymentRegister::HandleListExperiments, this, std::placeholders::_1));
}

void DeploymentRegister::Start(){
    replier_->Start();
}

void DeploymentRegister::Terminate(){
    replier_->Terminate();

    re_handlers_.clear();
    logan_handlers_.clear();
}

std::unique_ptr<NodeManager::NodeManagerRegistrationReply> DeploymentRegister::HandleNodeManagerRegistration(const NodeManager::NodeManagerRegistrationRequest& request){
    const auto& experiment_name = request.id().experiment_name();
    const auto& node_manager_ip_address = request.id().ip_address();

    //Check if Experiment Exists
    if(!environment_->GotExperiment(experiment_name)){
        std::string error("Haven't got experiment with name: " + experiment_name);
        std::cerr << "* DeploymentRegister::HandleNodeManagerRegistration: " << error << std::endl;
        throw std::runtime_error(error);
    }

    //Check if Experiment Exists and isn't already running
    if(!environment_->IsExperimentConfigured(experiment_name)){
        std::string error("Experiment: '" + experiment_name + "' isn't in configured state.");
        std::cerr << "* DeploymentRegister::HandleNodeManagerRegistration: " << error << std::endl;
        throw std::runtime_error(error);
    }

    auto& experiment = environment_->GetExperiment(experiment_name);
    auto& node = experiment.GetNode(node_manager_ip_address);
    
    auto component_count = node.GetDeployedComponentCount();
    

    auto reply = std::unique_ptr<NodeManager::NodeManagerRegistrationReply>(new NodeManager::NodeManagerRegistrationReply());


    if(component_count){
        if(node.IsNodeManagerMaster()){
            std::promise<std::string> port_promise;
            auto port_future = port_promise.get_future();

            auto handler = std::unique_ptr<DeploymentHandler>(new DeploymentHandler(*environment_,
                            environment_manager_ip_address_,
                            EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER,
                            node_manager_ip_address,
                            std::move(port_promise),
                            experiment_name));

            re_handlers_.emplace_back(std::move(handler));

            try{
                //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
                reply->set_heartbeat_endpoint(zmq::TCPify(environment_manager_ip_address_, port_future.get()));
                reply->add_types(NodeManager::NodeManagerRegistrationReply::MASTER);
            }
            catch(const std::exception& ex){
                throw;
            }
        }

        reply->add_types(NodeManager::NodeManagerRegistrationReply::SLAVE);

        //Set the endpoint information for both the SLAVE/MASTER
        reply->set_master_publisher_endpoint(experiment.GetMasterPublisherAddress());
        reply->set_master_registration_endpoint(experiment.GetMasterRegistrationAddress());
    }

    return std::move(reply);
}

std::unique_ptr<NodeManager::LoganRegistrationReply> DeploymentRegister::HandleLoganRegistration(const NodeManager::LoganRegistrationRequest& request){
    const auto& experiment_name = request.id().experiment_name();
    const auto& logan_server_ip_address = request.id().ip_address();
    
    std::promise<std::string> port_promise;
    auto port_future = port_promise.get_future();

    if(!environment_->GotExperiment(experiment_name)){
        throw std::runtime_error("No experiment with name: '" + experiment_name + "'");
    }

    try{
        auto& experiment = environment_->GetExperiment(experiment_name);
        auto logging_servers = experiment.GetAllocatedLoganServers(logan_server_ip_address);

        auto reply = std::unique_ptr<NodeManager::LoganRegistrationReply>(new NodeManager::LoganRegistrationReply());
        if(logging_servers.size() > 0){
            auto handler = std::unique_ptr<DeploymentHandler>(new DeploymentHandler(*environment_,
                                        environment_manager_ip_address_,
                                        EnvironmentManager::Environment::DeploymentType::LOGAN_SERVER,
                                        logan_server_ip_address,
                                        std::move(port_promise),
                                        experiment_name));
            logan_handlers_.emplace_back(std::move(handler));
            //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
            reply->set_heartbeat_endpoint(zmq::TCPify(environment_manager_ip_address_, port_future.get()));
        }

        for(auto& logger : logging_servers){
            reply->add_logger()->Swap(logger.get());
        }

        return std::move(reply);
    }
    catch(const std::exception& ex){
        throw;
    }
}

std::unique_ptr<EnvironmentControl::ShutdownExperimentReply> DeploymentRegister::HandleShutdownExperiment(const EnvironmentControl::ShutdownExperimentRequest& message){
    auto reply_message = std::unique_ptr<EnvironmentControl::ShutdownExperimentReply>(new EnvironmentControl::ShutdownExperimentReply());
    environment_->ShutdownExperiment(message.experiment_name());
    return reply_message;
}

std::unique_ptr<EnvironmentControl::ListExperimentsReply> DeploymentRegister::HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message){
    auto reply_message = std::unique_ptr<EnvironmentControl::ListExperimentsReply>(new EnvironmentControl::ListExperimentsReply());
    auto experiment_names = environment_->GetExperimentNames();
    *reply_message->mutable_experiment_names() = {experiment_names.begin(), experiment_names.end()};
    return reply_message;
}

std::unique_ptr<NodeManager::RegisterExperimentReply> DeploymentRegister::HandleRegisterExperiment(const NodeManager::RegisterExperimentRequest& request){
    environment_->PopulateExperiment(request.control_message());
    return environment_->GetExperimentDeploymentInfo(request.id().experiment_name());
};