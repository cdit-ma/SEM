#ifndef RECIEVERIMPL_H
#define RECIEVERIMPL_H

#include "interfaces.h"

class RecieverImpl: public RecieverInt{
    public:
        void rxMessage(Message* message);
        rxMessageInt* rxMessage_;
};

#endif //RECIEVERIMPL_H
