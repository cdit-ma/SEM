#ifndef RECEIVERIMPL_H
#define RECEIVERIMPL_H

#include "interfaces.h"

class ReceiverImpl: public ReceiverInt{
    public:
        ReceiverImpl(std::string name);
        void rxMessage(Message* message);
        void rxVectorMessage(VectorMessage* message);
};

#endif //RECEIVERIMPL_H
