#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "activatable.h"
#include <map>
#include "attribute.h"

//Interface for a standard EventPort
class EventPort: public Activatable{
    virtual void startup(std::map<std::string, ::Attribute*> attributes) = 0;
    virtual void teardown() = 0;
};

#endif //EVENTPORT_H