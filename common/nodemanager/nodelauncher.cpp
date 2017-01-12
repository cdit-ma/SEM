#include <dlfcn.h>
#include <iostream>
#include <string>

#include "../includes/core/nodecontainer.h"

int main(int argc, char **argv)
{
    //Get the library path from the argument variables
    char* lib_path = 0;
    if(argc == 2){
        lib_path = argv[1];
    }

    if(!lib_path){
        std::cerr << "DLL Error: No DLL path provided" << std::endl;
        exit(1);
    }

    //Get a handle to the dynamically linked library
    void* lib_handle = dlopen(lib_path, RTLD_LAZY);
    
    char* error;

    if (!lib_handle){
        //If we can't open the dll, get the error message and quit
        error = dlerror();
        std::cerr << "DLL Error: " << error << std::endl;
        exit(1);
    }
    
    //Declare function to the functions contained in the dynamic library loaded.
    auto create_deployment = (NodeContainer* (*)()) dlsym(lib_handle, "create_deployment");
    auto destroy_deployment = (void (*)(NodeContainer*)) dlsym(lib_handle, "destroy_deployment");

    //Check for errors
    error = dlerror();
    if(error != NULL){
        std::cerr << "DLL Error: " << error << std::endl;
        exit(1);
    }

    //Construct an instance of the Deployment
    NodeContainer* instance = create_deployment();
    
    //Start deployment instance
    instance->startup();
    
    bool running = true;

    while(running){
        std::cout << "Enter Instruction: ";
        std::string command;
        std::getline(std::cin, command);
        
        if(command == "activate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                instance->activate_all();
            }else{
                instance->activate(name);
            }
        }else if(command == "passivate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                instance->passivate_all();
            }else{
                instance->passivate(name);
            }
        }else if(command == "quit"){
            running = false;
        }
    }
  
    //Teardown deployment instance
    instance->passivate_all();
    instance->teardown();
    //Free Memory
    destroy_deployment(instance);
    
    return 0;
}