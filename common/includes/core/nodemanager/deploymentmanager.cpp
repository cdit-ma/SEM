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
    //std::cout << "DM PA: " << action << std::endl;
    auto cm = new NodeManager::ControlMessage();

    if(cm->ParseFromString(action) && deployment_){
        switch(cm->type()){
            case NodeManager::ControlMessage::STARTUP:{
                std::cout << "STARTING UP!" << std::endl;
                deployment_->startup();

                std::cout << "CONFIGURING!" << std::endl;
                std::cout << cm->DebugString() << std::endl;
                deployment_->configure(cm);
            
                
                
            break;
            }
            case NodeManager::ControlMessage::ACTIVATE:
                std::cout << "ACTIVATE!" << std::endl;
                deployment_->activate_all();
                break;
            case NodeManager::ControlMessage::PASSIVATE:
                std::cout << "PASSIVATE" << std::endl;
                deployment_->passivate_all();
            default:
                break;
        }
    }
}


bool DeploymentManager::is_library_loaded(){
    return library_handle_ && create_deployment_ && destroy_deployment_;
}

NodeContainer* DeploymentManager::get_deployment(){
    if(is_library_loaded() && !deployment_){
        deployment_ = create_deployment_();
    }
    return deployment_;
}
