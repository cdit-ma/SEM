#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "activatable.h"
#include <map>
#include "attribute.h"

//Forward declare
class Component;

//Interface for a standard EventPort
class EventPort: public Activatable{
    public:
        enum class Kind{
            TX = 0,
            RX = 1,
            PE = 2
        };

        EventPort(Component* component, std::string name, EventPort::Kind kind);
        virtual ~EventPort(){};
        virtual void Startup(std::map<std::string, ::Attribute*> attributes) = 0;
        
        virtual bool Activate();
        virtual bool Passivate();
        virtual bool Teardown() = 0;

        bool IsOutEventPort() const;
        bool IsInEventPort() const;
        bool IsPeriodicEvent() const;
        EventPort::Kind get_kind() const;

        Component* get_component();
    private:
        
        EventPort::Kind kind_;
        Component* component_;
};

#endif //EVENTPORT_H