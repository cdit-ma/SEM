#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <map>
#include <mutex>

#include "activatable.h"
#include "attribute.h"
#include "eventport.h"
#include "basemessage.h"


class Component: public Activatable{
    public:
        Component(std::string inst_name);
        virtual ~Component();
        bool Activate();
        bool Passivate();

        

        void AddEventPort(EventPort* event_port);
        void RemoveEventPort(EventPort* event_port);
        EventPort* GetEventPort(std::string name);

        void AddAttribute(Attribute* attribute);
        Attribute* GetAttribute(std::string name);

        void AddCallback(std::string port_name, std::function<void (::BaseMessage*)> function);
        std::function<void (::BaseMessage*)> GetCallback(std::string port_name);
    private:
        std::mutex mutex_;
        std::map<std::string, EventPort*> eventports_;
        std::map<std::string, Attribute*> attributes_;    
        std::map<std::string, std::function<void (::BaseMessage*)> > callback_functions_;
        std::string inst_name_;
};

#endif //COMPONENT_H