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
    subscriber_->Start();
    
    //Subscribe to NodeManager::ControlMessage Types
    auto cm_callback = std::bind(&DeploymentManager::ProcessControlMessage, this, std::placeholders::_1);
    subscriber_->RegisterNewProto(NodeManager::ControlMessage::default_instance(), cm_callback);
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
    return ModelLogger::setup_model_logger(host_name, pub_endpoint, false);
}

void DeploymentManager::ProcessControlMessage(google::protobuf::MessageLite* ml){
    NodeManager::ControlMessage* control_message = (NodeManager::ControlMessage*)ml;
    std::cout << "Got control message!" << std::endl;
}

void DeploymentManager::ProcessAction(std::string node_name, std::string action){
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Processing Action: " << node_name << std::endl;

    auto cm = new NodeManager::ControlMessage();
    
    if(cm->ParseFromString(action)){
        std::cout << "Process Action: " << node_name << " ACTION: " << NodeManager::ControlMessage_Type_Name(cm->type()) << std::endl;
        switch(cm->type()){
            case NodeManager::ControlMessage::STARTUP:{
                if(!deployment_){
                    deployment_ = new NodeContainer(library_path_);    
                    deployment_->Configure(cm);
                    deployment_->ActivateAll();
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

}

NodeContainer* DeploymentManager::get_deployment(){
    return deployment_;
}
