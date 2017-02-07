#include "deploymentmanager.h"
#include <dlfcn.h>
#include <iostream>
#include "../translate.h"

#include "controlmessage.pb.h"

DeploymentManager::DeploymentManager(std::string library_path){
    library_path_ = library_path;
}

DeploymentManager::~DeploymentManager(){
    if(deployment_){
        delete deployment_;
    }
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
