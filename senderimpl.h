#ifndef SENDERIMPL_H
#define SENDERIMPL_H

#include "interfaces.h"

class SenderImpl: public SenderInt{
    public:
        SenderImpl(std::string name);

        void periodic_event();
        void periodic_event_v();
    private:
        //Variable
        int sentCount_;
};

#endif //SENDERIMPL_H
