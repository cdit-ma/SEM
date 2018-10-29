#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>
#include <zmq.hpp>
#include <zmq/zmqutils.hpp>
#include <proto/controlmessage/helper.h>
#include "node.h"
#include "container.h"

DeploymentRegister::DeploymentRegister(Execution& execution, const std::string& environment_manager_ip_address, const std::string& registration_port, 
                                        const std::string& qpid_broker_address, const std::string& tao_naming_server_address,
                                        int portrange_min, int portrange_max)
                                        :
execution_(execution),
environment_manager_ip_address_(environment_manager_ip_address)
{

    assert(portrange_min < portrange_max);
    environment_ = std::unique_ptr<EnvironmentManager::Environment>(new EnvironmentManager::Environment(environment_manager_ip_address, qpid_broker_address, tao_naming_server_address, portrange_min, portrange_max));

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

    replier_->RegisterProtoCallback<NodeManager::InspectExperimentRequest, NodeManager::InspectExperimentReply>
            ("InspectExperiment",
             std::bind(&DeploymentRegister::HandleInspectExperiment, this, std::placeholders::_1));

    replier_->Start();


    cleanup_future_ = std::async(std::launch::async, &DeploymentRegister::CleanupTask, this);

}

DeploymentRegister::~DeploymentRegister(){
    Terminate();
    environment_.reset();
}

void DeploymentRegister::Terminate(){
    if(replier_){
        replier_->Terminate();
        replier_.reset();
    }

    {
        std::lock_guard<std::mutex> lock(termination_mutex_);
        terminated_ = true;
        cleanup_cv_.notify_all();
    }

    if(cleanup_future_.valid()){
        cleanup_future_.get();
    }

    {
        std::lock_guard<std::mutex> lock(handler_mutex_);
        handlers_.clear();
    }
}

std::unique_ptr<NodeManager::NodeManagerRegistrationReply> DeploymentRegister::HandleNodeManagerRegistration(const NodeManager::NodeManagerRegistrationRequest& request){
    const auto& experiment_name = request.id().experiment_name();
    const auto& node_manager_ip_address = request.id().ip_address();
    const auto& container_id = request.container_id();

    //Check if Experiment Exists
    if(!environment_->GotExperiment(experiment_name)){
        std::string error("Haven't got experiment with name: " + experiment_name);
        std::cerr << "* DeploymentRegister::HandleNodeManagerRegistration: " << error << std::endl;
        throw std::runtime_error(error);
    }

    auto& experiment = environment_->GetExperiment(experiment_name);
    auto& node = experiment.GetNode(node_manager_ip_address);
    auto& container = node.GetContainer(container_id);
    auto component_count = container.GetDeployedCount();

    auto reply = std::unique_ptr<NodeManager::NodeManagerRegistrationReply>(new NodeManager::NodeManagerRegistrationReply());

    if(component_count){
        if(container.IsNodeManagerMaster()){
            //Check if Experiment Exists and isn't already running
            if(!environment_->IsExperimentConfigured(experiment_name)){
                std::string error("Experiment: '" + experiment_name + "' isn't in configured state.");
                std::cerr << "* DeploymentRegister::HandleNodeManagerRegistration: " << error << std::endl;
                throw std::runtime_error(error);
            }
            std::promise<std::string> port_promise;
            auto port_future = port_promise.get_future();

            auto handler = std::unique_ptr<DeploymentHandler>(new DeploymentHandler(*environment_,
                            environment_manager_ip_address_,
                            EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER,
                            node_manager_ip_address,
                            std::move(port_promise),
                            experiment_name));

            {
                std::lock_guard<std::mutex> lock(handler_mutex_);
                handlers_.emplace_back(std::move(handler));
            }

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
        if(!logging_servers.empty()){
            auto handler = std::unique_ptr<DeploymentHandler>(new DeploymentHandler(*environment_,
                                        environment_manager_ip_address_,
                                        EnvironmentManager::Environment::DeploymentType::LOGAN_SERVER,
                                        logan_server_ip_address,
                                        std::move(port_promise),
                                        experiment_name));
            {
                std::lock_guard<std::mutex> lock(handler_mutex_);
                handlers_.emplace_back(std::move(handler));
            }
            //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
            reply->set_heartbeat_endpoint(zmq::TCPify(environment_manager_ip_address_, port_future.get()));
        }

        for(auto& logger : logging_servers){
            reply->mutable_logger()->AddAllocated(logger.release());
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
    environment_->PopulateExperiment(request.experiment());
    auto reply = environment_->GetExperimentDeploymentInfo(request.id().experiment_name());
    return reply;
}

// TODO: Fix this, invalidates the dirty flag on experiments.
std::unique_ptr<NodeManager::InspectExperimentReply> DeploymentRegister::HandleInspectExperiment(const NodeManager::InspectExperimentRequest& request){
    auto reply = std::unique_ptr<NodeManager::InspectExperimentReply>(new NodeManager::InspectExperimentReply());
    reply->set_allocated_environment_message(environment_->GetProto(request.experiment_name(), true).release());
    return reply;
}

void DeploymentRegister::CleanupTask(){
    while(true){
        std::unique_lock<std::mutex> termination_lock(termination_mutex_);
        cleanup_cv_.wait_for(termination_lock, std::chrono::milliseconds(cleanup_period_), [this]{return terminated_;});

        if(terminated_){
           break;
        }

        std::lock_guard<std::mutex> handler_lock(handler_mutex_);
        handlers_.erase(
                std::remove_if(handlers_.begin(), handlers_.end(), [](const std::unique_ptr<DeploymentHandler>& handler){return handler->IsRemovable();}), handlers_.end());
    }
}