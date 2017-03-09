#include "deploymentmanager.h"


#include <iostream>

#include "../../re_common/zmq/protoreceiver/protoreceiver.h"
#include "../controlmessage/controlmessage.pb.h"
#include "../controlmessage/translate.h"
#include "../modellogger.h"

DeploymentManager::DeploymentManager(std::string library_path){
    library_path_ = library_path;

    //Construct a live receiever
    subscriber_ = new zmq::ProtoReceiver();
    //Get all Main messages
    subscriber_->Filter("*");
    
    //Subscribe to NodeManager::ControlMessage Types
    auto cm_callback = std::bind(&DeploymentManager::GotControlMessage, this, std::placeholders::_1);
    subscriber_->RegisterNewProto(NodeManager::ControlMessage::default_instance(), cm_callback);

    subscriber_->Start();
}

DeploymentManager::~DeploymentManager(){
    if(deployment_){
        delete deployment_;
    }
}

bool DeploymentManager::SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name){
    if(subscriber_){
        std::cout << "Subscribing to: " << pub_endpoint << " Filter: " << host_name << "*" << std::endl;
        subscriber_->Connect(pub_endpoint);
        subscriber_->Filter(host_name + "*");
        return true;
    }
    return false;
}
bool DeploymentManager::SetupModelLogger(std::string pub_endpoint, std::string host_name){
    return ModelLogger::setup_model_logger(host_name, pub_endpoint, true);
}

bool DeploymentManager::TeardownModelLogger(){
    return ModelLogger::shutdown_logger();
}

void DeploymentManager::GotControlMessage(google::protobuf::MessageLite* ml){
    NodeManager::ControlMessage* control_message = (NodeManager::ControlMessage*)ml;
    if(control_message){
        ProcessControlMessage(control_message);
    }
}

void DeploymentManager::ProcessControlMessage(NodeManager::ControlMessage* cm){
    std::lock_guard<std::mutex> lock(mutex_);

    switch(cm->type()){
        case NodeManager::ControlMessage::STARTUP:{
            if(!deployment_){
                deployment_ = new NodeContainer(library_path_);    
                deployment_->Configure(cm);
            }
            break;
        }
        case NodeManager::ControlMessage::ACTIVATE:
            if(deployment_){
                deployment_->ActivateAll();
            }
            break;
        case NodeManager::ControlMessage::PASSIVATE:
            if(deployment_){
                deployment_->PassivateAll();
            }
            break;
        case NodeManager::ControlMessage::TERMINATE:
            if(deployment_){
                deployment_->Teardown();
                delete deployment_;
                deployment_ = 0;
            }
            break;
        case NodeManager::ControlMessage::SET_ATTRIBUTE:
            if(deployment_){
                deployment_->Configure(cm);
            }
            break;
        default:
            break;
    }
}

NodeContainer* DeploymentManager::get_deployment(){
    return deployment_;
}
