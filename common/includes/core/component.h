#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <deque>

#include "activatable.h"
#include "eventport.h"

class Component: public Activatable{
    public:
        Component(std::string inst_name);
        const std::string get_name();

        bool activate();
        bool passivate();

        void add_event_port(EventPort* event_port);
        void remove_event_port(EventPort* event_port);

    private:
        std::deque<EventPort*> eventports_;    
        std::string inst_name_;
};

#endif //COMPONENT_H