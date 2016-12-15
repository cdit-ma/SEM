#ifndef receiveRIMPL_H
#define receiveRIMPL_H

#include "interfaces.h"

class receiverImpl: public receiverInt{
    public:
        void rxMessage(Message* message);
};

#endif //receiveRIMPL_H
