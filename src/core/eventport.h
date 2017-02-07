#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "activatable.h"
#include <map>
#include "attribute.h"


//Interface for a standard EventPort
class EventPort: public Activatable{
    public:
        virtual ~EventPort(){};
        virtual void Startup(std::map<std::string, ::Attribute*> attributes) = 0;
        virtual void Teardown() = 0;
};

#endif //EVENTPORT_H