#ifndef NODECONTAINER_H
#define NODECONTAINER_H

#include <unordered_map>
#include <functional>

#include "component.h"

namespace NodeManager{
    class ControlMessage;
};

typedef std::function<EventPort* (std::string, Component*, std::string)> TxConstructor;
typedef std::function<EventPort* (std::string, Component*, std::string)> RxConstructor;
typedef std::function<Component* (std::string)> ComponentConstructor;

class NodeContainer{ 
    public:
        NodeContainer(std::string library_path);
        ~NodeContainer();
        
        bool activate(std::string component_name);
        bool passivate(std::string component_name);

        bool activate_all();
        bool passivate_all();

        void configure(NodeManager::ControlMessage* message);
        
        void teardown();

        bool add_component(Component* component);
        Component* get_component(std::string component_name);

    private:
        //DLL functions
        void* load_library(std::string dll_path);
        void* get_library_function(std::string dll_path, std::string function_name);
        void* get_library_function(void* lib_handle, std::string function_name);

        void add_tx_constructor(std::string middleware, TxConstructor constructor);
        void add_rx_constructor(std::string middleware, TxConstructor constructor);
        void add_component_constructor(std::string component_type, ComponentConstructor constructor);
    
    public:
        //Constructor functions
        EventPort* construct_periodic_event(Component* component, std::string port_name);
        EventPort* construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name);
        EventPort* construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name);

        Component* construct_component(std::string component_type, std::string component_name);

     private:
        std::string library_path_;

        //Library Path -> void *
        std::unordered_map<std::string, void *> loaded_libraries_;

        //Middleware -> construct functions
        std::unordered_map<std::string, TxConstructor> tx_constructors_;
        std::unordered_map<std::string, RxConstructor> rx_constructors_;

        //Component Type -> construct functions
        std::unordered_map<std::string, ComponentConstructor> component_constructors_;
        
        //Map for quick insertion and stuffs.
        std::map<std::string, Component*> components_;
};
      

#endif //NODECONTAINER_H