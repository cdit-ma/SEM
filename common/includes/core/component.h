#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <map>

#include "activatable.h"
#include "eventport.h"

class Component: public Activatable{
    public:
        Component(std::string inst_name);

        bool activate();
        bool passivate();

        void add_event_port(EventPort* event_port);
        void remove_event_port(EventPort* event_port);
        EventPort* get_event_port(std::string name);

    private:
        std::map<std::string, EventPort*> eventports_;    
        std::string inst_name_;
};

#endif //COMPONENT_H