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
        enum class Type{
            TX = 0,
            RX = 1,
            PE = 2
        };

        EventPort(Component* component, std::string name, EventPort::Type type);
        virtual ~EventPort(){};
        virtual void Startup(std::map<std::string, ::Attribute*> attributes) = 0;
        virtual void Teardown() = 0;

        virtual bool Activate();
        virtual bool Passivate();

        const bool IsOutEventPort();
        const bool IsInEventPort();
        const bool IsPeriodicEvent();
        const EventPort::Type get_type();

        Component* get_component();
    private:
        EventPort::Type type_;
        Component* component_;
};

#endif //EVENTPORT_H