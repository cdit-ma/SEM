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
        void set_library_path(std::string library_path);
        bool activate(std::string component_name);
        bool passivate(std::string component_name);

        bool activate_all();
        bool passivate_all();

        void configure(NodeManager::ControlMessage* message);
        
        virtual void startup(){};
        //virtual ::EventPort* construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name) = 0;
        //virtual ::EventPort* construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name) = 0;;
        ::EventPort* construct_periodic_event(Component* component, std::string port_name);
        //virtual ::Component* construct_component(std::string componentType, std::string component_name) = 0;

        void teardown();

        bool add_component(Component* component);
        Component* get_component(std::string component_name);

        //
        EventPort* construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name);
        EventPort* construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name);
        Component* construct_component(std::string component_type, std::string component_name);

        //Adders
        void add_tx_constructor(std::string middleware, TxConstructor constructor);
        void add_rx_constructor(std::string middleware, TxConstructor constructor);
        void add_component_constructor(std::string component_type, ComponentConstructor constructor);

     private:
        std::string library_path_;
        std::unordered_map<std::string, TxConstructor> TxConstructors_;
        std::unordered_map<std::string, RxConstructor> RxConstructors_;
        std::unordered_map<std::string, ComponentConstructor> ComponentConstructors_;

        std::unordered_map<std::string, Component*> components_;
};
      

#endif //NODECONTAINER_H