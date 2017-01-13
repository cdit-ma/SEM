#include "deploymentmanager.h"
#include <dlfcn.h>
#include <iostream>

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

bool DeploymentManager::is_library_loaded(){
    return library_handle_ && create_deployment_ && destroy_deployment_;
}

NodeContainer* DeploymentManager::get_deployment(){
    if(is_library_loaded() && !deployment_){
        deployment_ = create_deployment_();
    }
    return deployment_;
}
