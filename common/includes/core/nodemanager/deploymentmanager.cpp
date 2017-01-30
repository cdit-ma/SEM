#include "deploymentmanager.h"
#include <dlfcn.h>
#include <iostream>
#include "../translate.h"

#include "controlmessage.pb.h"

DeploymentManager::DeploymentManager(std::string library_path){
    library_path_ = library_path;
    
    if(!library_path_.empty()){
        char* error;
        //Get a handle to the dynamically linked library
        library_handle_ = dlopen(library_path_.c_str(), RTLD_LAZY);
        
        //Check for errors
        error = dlerror();
        if(error){
            std::cerr << "DLL Error: " << error << std::endl;
            return;
        }

        if(library_handle_){
            //Link the create_deployment function
            create_deployment_ = (NodeContainer* (*)()) dlsym(library_handle_, "create_deployment");
            error = dlerror();
            if(error){
                std::cerr << "DLL Error Linking 'create_deployment': " << error << std::endl;
                return;
            }

            destroy_deployment_ = (void (*)(NodeContainer*)) dlsym(library_handle_, "destroy_deployment");
            error = dlerror();
            if(error){
                std::cerr << "DLL Error Linking 'create_deployment': " << error << std::endl;
                return;
            }
        }
        std::cout << "Successfully Loaded Library:" << library_path_ << std::endl;
    }

}

DeploymentManager::~DeploymentManager(){
    if(library_handle_){
        //Close the library
        dlclose(library_handle_);
    }
}

void DeploymentManager::process_action(std::string node_name, std::string action){
    auto cm = new NodeManager::ControlMessage();
    
    if(cm->ParseFromString(action) && deployment_){
        std::cout << "Process Action: " << node_name << " ACTION: " << NodeManager::ControlMessage_Type_Name(cm->type()) << std::endl;
        switch(cm->type()){
            case NodeManager::ControlMessage::STARTUP:{
                deployment_->startup();
                deployment_->configure(cm);
                break;
            }
            case NodeManager::ControlMessage::ACTIVATE:
                deployment_->activate_all();
                break;
            case NodeManager::ControlMessage::PASSIVATE:
                deployment_->passivate_all();
                break;
            case NodeManager::ControlMessage::TERMINATE:
                deployment_->teardown();
                break;
            case NodeManager::ControlMessage::SET_ATTRIBUTE:
                deployment_->configure(cm);
                break;
            default:
                break;
        }
    }
}


bool DeploymentManager::is_library_loaded(){
    return library_handle_ && create_deployment_ && destroy_deployment_;
}

NodeContainer* DeploymentManager::get_deployment(){
    if(!deployment_){
        deployment_ = new NodeContainer();    
        deployment_->set_library_path(library_path_);
    }
    //deployment_ = ne
    //if(is_library_loaded() && !deployment_){
    //    deployment_ = create_deployment_();
    //}
    return deployment_;
}
