#ifndef NODECONTAINER_H
#define NODECONTAINER_H

#include <unordered_map>

#include "component.h"

namespace NodeManager{
    class ControlMessage;
};

class NodeContainer{ 
    public:
        bool activate(std::string component_name);
        bool passivate(std::string component_name);

        bool activate_all();
        bool passivate_all();

        void configure(NodeManager::ControlMessage* message);
        
        virtual void startup() = 0;
        virtual ::EventPort* construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name) = 0;
        virtual ::EventPort* construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name) = 0;;
        ::EventPort* construct_periodic_event(Component* component, std::string port_name);
        virtual ::Component* construct_component(std::string componentType, std::string component_name) = 0;

        void teardown();

        bool add_component(Component* component);
        Component* get_component(std::string component_name);
     private:
        std::unordered_map<std::string, Component*> components_;
};
      

#endif //NODECONTAINER_H