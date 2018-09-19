#include "deploymenthandler.h"
#include <iostream>

DeploymentHandler::DeploymentHandler(EnvironmentManager::Environment& env,
                                    const std::string& ip_addr,
                                    EnvironmentManager::Environment::DeploymentType deployment_type,
                                    const std::string& deployment_ip_address,
                                    std::promise<std::string>* port_promise,
                                    const std::string& experiment_id) :
                                    environment_(env){

    ip_addr_ = ip_addr;
    deployment_type_ = deployment_type;
    deployment_ip_address_ = deployment_ip_address;
    port_promise_ = port_promise;
    experiment_id_ = experiment_id;


    handler_thread_ = std::unique_ptr<std::thread>(new std::thread(&DeploymentHandler::HeartbeatLoop, this));
}

void DeploymentHandler::PrintError(const std::string& message){
    std::cerr << "* ";
    if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER){
        std::cerr << "Runtime Environment: ";
    }else if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT){
        std::cerr << "Logan Server: ";
    }
    std::cerr << "Experiment[" << experiment_id_ << "] IP: " << ip_addr_  << ": " << message << std::endl;
}   

void DeploymentHandler::Terminate(){
    //TODO: (RE-253) send TERMINATE messages to node manager attached to this thread
    replier_->Terminate();
}

void DeploymentHandler::HeartbeatLoop() noexcept{

    replier_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
    const auto& assigned_port = environment_.AddDeployment(experiment_id_, deployment_ip_address_, deployment_type_);

    try{
        replier_->Bind(TCPify(ip_addr_, assigned_port));
        port_promise_->set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise_->set_exception(std::current_exception());
        RemoveDeployment();
        return;
    }

    std::vector<std::chrono::milliseconds> timeouts;
    timeouts.push_back(std::chrono::milliseconds(2000));
    timeouts.push_back(std::chrono::milliseconds(4000));
    timeouts.push_back(std::chrono::milliseconds(8000));

    replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                ("EnvironmentManagerHeartbeat", std::bind(&DeploymentHandler::HandleRequest, this, std::placeholders::_1));

    replier_->Start(timeouts);
    
    RemoveDeployment();
}

void DeploymentHandler::RemoveDeployment() noexcept{
    try{
        if(!removed_flag_){
            if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER){
                environment_.RemoveExperiment(experiment_id_);
            }
            if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT){
                environment_.RemoveLoganClientServer(experiment_id_, deployment_ip_address_);
            }
            removed_flag_ = true;
        }
    }
    catch(const std::exception& ex){
        PrintError(std::string("Exception: HeartbeatLoop (Initial): ") + ex.what());
    }
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

std::unique_ptr<NodeManager::EnvironmentMessage> DeploymentHandler::HandleRequest(const NodeManager::EnvironmentMessage& request_message){
    auto message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage(request_message));

    try{
        switch(message->type()){
            case NodeManager::EnvironmentMessage::GET_DEPLOYMENT_INFO:{
                //Register the Experiment, which will modify the control_message
                environment_.PopulateExperiment(*(message->mutable_control_message()));
                message->set_type(NodeManager::EnvironmentMessage::SUCCESS);
                break;
            }

            case NodeManager::EnvironmentMessage::REMOVE_DEPLOYMENT:{
                RemoveDeployment();
                message->set_type(NodeManager::EnvironmentMessage::SUCCESS);
                break;
            }

            case NodeManager::EnvironmentMessage::HEARTBEAT:{
                message->set_type(NodeManager::EnvironmentMessage::HEARTBEAT_ACK);

                if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER){
                    try{
                        if(environment_.ExperimentIsDirty(experiment_id_)){
                            HandleDirtyExperiment(*message);
                        }
                    }catch(const std::exception& ex){
                        PrintError(std::string("Failed to Update Dirty Experiment: ") + ex.what());
                        message->set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
                    }
                }else if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT){
                    std::lock_guard<std::mutex> lock(logan_ip_mutex_);
                    if(registered_logan_ip_addresses.count(ip_addr_)){
                        //If we were registered, now we don't have the experiment anymore, Terminate
                        if(!environment_.GotExperiment(experiment_id_)){
                            message->set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
                        }
                    }
                }
                break;
            }

            case NodeManager::EnvironmentMessage::LOGAN_QUERY:{
                HandleLoganQuery(*message);
                break;
            }

            default:{
                std::cerr << message->DebugString() << std::endl;
                message->set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
                break;
            }
        }
    }
    catch(std::exception& ex){
        PrintError("Failed to HandleRequest[" + NodeManager::EnvironmentMessage_Type_Name(message->type()) + "]: " + ex.what());
        message->set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
        message->add_error_messages(std::string(ex.what()));
    }

    return std::move(message);
}

void DeploymentHandler::HandleDirtyExperiment(NodeManager::EnvironmentMessage& message){
    message.set_type(NodeManager::EnvironmentMessage::UPDATE_DEPLOYMENT);

    if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER){
        auto experiment_update = environment_.GetExperimentUpdate(experiment_id_);
        if(experiment_update){
            message.set_allocated_control_message(experiment_update);
        }
    }

    if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT){
        environment_.ExperimentIsDirty(experiment_id_);
    }
}

void DeploymentHandler::HandleLoganQuery(NodeManager::EnvironmentMessage& message){
    message.set_type(NodeManager::EnvironmentMessage::LOGAN_QUERY);


    if(environment_.IsExperimentConfigured(message.experiment_id())){
        std::lock_guard<std::mutex> lock(logan_ip_mutex_);
        if(!registered_logan_ip_addresses.count(ip_addr_)){
            auto environment_message = environment_.GetLoganDeploymentMessage(message.experiment_id(), message.update_endpoint());
            if(environment_message){
                message.Swap(environment_message);
                delete environment_message;
                message.set_type(NodeManager::EnvironmentMessage::LOGAN_RESPONSE);
                registered_logan_ip_addresses.insert(ip_addr_);
            }
        }else{
            message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
        }
    }
    
}