#include "deploymenthandler.h"
#include <iostream>
#include <zmq/zmqutils.hpp>

DeploymentHandler::DeploymentHandler(EnvironmentManager::Environment& env,
                                    const std::string& ip_addr,
                                    EnvironmentManager::Environment::DeploymentType deployment_type,
                                    const std::string& deployment_ip_address,
                                    std::promise<std::string> port_promise,
                                    const std::string& experiment_id) :
environment_(env),
environment_manager_ip_address_(ip_addr),
deployment_type_(deployment_type),
deployment_ip_address_(deployment_ip_address),
experiment_id_(experiment_id)
{
    std::lock_guard<std::mutex> lock(replier_mutex_);
    replier_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
    const auto& assigned_port = environment_.AddDeployment(experiment_id_, deployment_ip_address_, deployment_type_);
    const auto& bind_address = zmq::TCPify(environment_manager_ip_address_, assigned_port);

    try{
        replier_->Bind(bind_address);
        port_promise.set_value(assigned_port);
    }catch(const zmq::error_t& ex){
        std::runtime_error error("Cannot bind ZMQ address: '" + bind_address + "'");
        //Set our promise as exception and exit if we can't find a free port.
        port_promise.set_exception(std::make_exception_ptr(error));
        throw error;
    }

    

    //Register the callbacks
    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                ("EnvironmentManagerHeartbeat", std::bind(&DeploymentHandler::HandleHeartbeat, this, std::placeholders::_1));
    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                ("EnvironmentManagerAddExperiment", std::bind(&DeploymentHandler::HandleAddExperiment, this, std::placeholders::_1));
    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                ("EnvironmentManagerRemoveExperiment", std::bind(&DeploymentHandler::HandleRemoveExperiment, this, std::placeholders::_1));
    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                ("EnvironmentManagerGetLoganInfo", std::bind(&DeploymentHandler::HandleGetLoganInfo, this, std::placeholders::_1));

    heartbeat_future_ = std::async(std::launch::async, &DeploymentHandler::HeartbeatLoop, this);
}

DeploymentHandler::~DeploymentHandler(){
    {
        std::lock_guard<std::mutex> lock(replier_mutex_);
        replier_->Terminate();
    }
    if(heartbeat_future_.valid()){
        heartbeat_future_.get();
    }
}

void DeploymentHandler::HeartbeatLoop() noexcept{
    std::future<void> replier_future;
    {
        std::vector<std::chrono::milliseconds> timeouts;
        timeouts.push_back(std::chrono::milliseconds(2000));
        timeouts.push_back(std::chrono::milliseconds(4000));
        timeouts.push_back(std::chrono::milliseconds(8000));

        std::lock_guard<std::mutex> lock(replier_mutex_);
        replier_future = replier_->Start(timeouts);
    }
    
    try{
        replier_future.get();
    }catch(const zmq::TimeoutException& ex){
        //Timeout Exceptions should lead to shutdown
        try{
            RemoveDeployment();
        }catch(const std::exception& ex){

        }
    }
}

void DeploymentHandler::RemoveDeployment(){
    if(!removed_flag_){
        if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER){
            environment_.RemoveExperiment(experiment_id_);
        }else if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT){
            environment_.RemoveLoganClientServer(experiment_id_, deployment_ip_address_);
        }
        removed_flag_ = true;
    }
}


std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentHandler::HandleHeartbeat(const NodeManager::EnvironmentMessage& request_message){
    auto return_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage());
    return_message->set_type(NodeManager::EnvironmentMessage::HEARTBEAT_ACK);
    
    if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER){
        if(environment_.ExperimentIsDirty(experiment_id_)){
            auto experiment_update = environment_.GetProto(experiment_id_, false);
            if(experiment_update){
                return_message.swap(experiment_update);
            }
        }
    }else if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT){
        std::lock_guard<std::mutex> lock(logan_ip_mutex_);
        if(registered_logan_ip_addresses.count(deployment_ip_address_)){
            //If we were registered, now we don't have the experiment anymore, Terminate
            if(!environment_.GotExperiment(experiment_id_)){
                return_message->set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
            }
        }
    }
    return return_message;
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentHandler::HandleAddExperiment(const NodeManager::EnvironmentMessage& request_message){
    auto deployment_info = environment_.PopulateExperiment(request_message.control_message());
    return deployment_info;
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentHandler::HandleRemoveExperiment(const NodeManager::EnvironmentMessage& request_message){
    RemoveDeployment();
    auto success = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage());
    return success;
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentHandler::HandleGetLoganInfo(const NodeManager::EnvironmentMessage& request_message){
    auto return_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage());
    return_message->set_type(NodeManager::EnvironmentMessage::LOGAN_QUERY);

    if(environment_.IsExperimentConfigured(experiment_id_)){
        std::lock_guard<std::mutex> lock(logan_ip_mutex_);
        if(!registered_logan_ip_addresses.count(deployment_ip_address_)){
            auto environment_message = environment_.GetLoganDeploymentMessage(experiment_id_, deployment_ip_address_);
            if(environment_message){
                return_message.swap(environment_message);
                //Register the logan_ip
                registered_logan_ip_addresses.insert(deployment_ip_address_);
            }
        }else{
            return_message->set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
        }
    }
    return return_message;
}