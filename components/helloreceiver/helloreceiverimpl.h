#ifndef COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H
#define COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H

#include "helloreceiverint.h"

class HelloReceiverImpl: public HelloReceiverInt{
    public:
        HelloReceiverImpl(std::string name);
        
        static Component* constructor_fn(std::string name);
        static void destructor_fn(Component* c);

        void rxMessage(Message* message);
};

extern "C" Component* construct_component(std::string name)
{
    return new HelloReceiverImpl(name);
};

#endif //COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H
