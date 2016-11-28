#ifndef SENDERIMPL_H
#define SENDERIMPL_H

#include "interfaces.h"
//#include "message.h"

class SenderImpl: public SenderInt{
    public:
        void periodic_event();

    public:
        void txMessage(Message* message);
        txMessageInt* txMessage_;
};

#endif //SENDERIMPL_H
