#include <dlfcn.h>
#include <iostream>
#include <string>
#include "common/includes/globalinterfaces.hpp"

int main(int argc, char **argv)
{
    char* lib_path;
    if(argc == 2){
        lib_path = argv[1];

    }
    //std::string lib_path = "./build/libtest_runtime.so";
    void* lib_handle = dlopen(lib_path, RTLD_LAZY);
    
    char * error;

    if (!lib_handle){
        error = dlerror();
        std::cerr << "DLL Error: " << error << std::endl;
        exit(1);
    }

    //Forward Declare the function pointers
    NodeContainer* (*create)();
    void (*destroy)(NodeContainer*);

    //Point our function pointers above to the functions inside the dynamic library loaded.
    create = (NodeContainer* (*)()) dlsym(lib_handle, "create_object");
    destroy = (void (*)(NodeContainer*)) dlsym(lib_handle, "destroy_object");

    //Check for errors
    error = dlerror();
    if(error != NULL){
        std::cerr << "DLL Error: " << error << std::endl;
        exit(1);
    }

    //Construct an instance of our NodeContainer from the dynamic library we have loaded.
    NodeContainer* instance = create();
    
    //Start our instance
    instance->startup();
    
    bool running = true;

    while(running){
        std::cout << "Enter Instruction: ";
        std::string command;
        std::getline(std::cin, command);
        
        if(command == "activate"){
            instance->activate();
        }else if(command == "passivate"){
            instance->passivate();
        }else if(command == "quit"){
            running = false;
        }
    }
  
    instance->passivate();
    instance->teardown();

    destroy(instance);
}