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
        void teardown();

        bool add_component(Component* component);
        Component* get_component(std::string component_name);
     private:
        std::unordered_map<std::string, Component*> components_;
};
      

#endif //NODECONTAINER_H