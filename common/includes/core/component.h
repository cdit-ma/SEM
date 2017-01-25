#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <map>

#include "activatable.h"
#include "attribute.h"
#include "eventport.h"
#include "basemessage.h"


class Component: public Activatable{
    public:
        Component(std::string inst_name);
        ~Component();

        bool activate();
        bool passivate();

        

        void add_event_port(EventPort* event_port);
        void remove_event_port(EventPort* event_port);
        EventPort* get_event_port(std::string name);

        void add_attribute(Attribute* attribute);
        Attribute* get_attribute(std::string name);

        void add_callback(std::string port_name, std::function<void (::BaseMessage*)> function);
        std::function<void (::BaseMessage*)> get_callback(std::string port_name);
    private:
        std::map<std::string, EventPort*> eventports_;
        std::map<std::string, Attribute*> attributes_;    
        std::map<std::string, std::function<void (::BaseMessage*)> > callback_functions_;
        std::string inst_name_;
};

#endif //COMPONENT_H