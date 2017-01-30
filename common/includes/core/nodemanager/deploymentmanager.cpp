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

void DeploymentManager::process_action(std::string node_name, std::string action){
    std::lock_guard<std::mutex> lock(mutex_);

    auto cm = new NodeManager::ControlMessage();
    
    if(cm->ParseFromString(action) && deployment_){
        std::cout << "Process Action: " << node_name << " ACTION: " << NodeManager::ControlMessage_Type_Name(cm->type()) << std::endl;
        switch(cm->type()){
            case NodeManager::ControlMessage::STARTUP:{
                if(!deployment_){
                    deployment_ = new NodeContainer(library_path_);    
                    deployment_->configure(cm);
                }
                break;
            }
            case NodeManager::ControlMessage::ACTIVATE:
                if(deployment_){
                    deployment_->activate_all();
                }
                break;
            case NodeManager::ControlMessage::PASSIVATE:
                if(deployment_){
                    deployment_->passivate_all();
                }
                break;
            case NodeManager::ControlMessage::TERMINATE:
                if(deployment_){
                    deployment_->teardown();
                    delete deployment_;
                    deployment_ = 0;
                }
                break;
            case NodeManager::ControlMessage::SET_ATTRIBUTE:
                if(deployment_){
                    deployment_->configure(cm);
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
