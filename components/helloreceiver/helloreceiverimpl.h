#ifndef COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H
#define COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H

#include "helloreceiverint.h"

class HelloReceiverImpl: public HelloReceiverInt{
    public:
        HelloReceiverImpl(std::string name);
        void rxMessage(Message* message);
        void rxVectorMessage(VectorMessage message);
        
};

#endif //COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H
