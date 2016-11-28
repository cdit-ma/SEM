#ifndef TESTTXMESSAGE_H
#define TESTTXMESSAGE_H

#include "../interfaces.h"

class test_txMessage: public txMessageInt{
    public:
        test_txMessage(txMessageInt* component, rxMessageInt* rxMessage);
        void txMessage(Message* message);
    private:
        rxMessageInt* rmMessage_;
        txMessageInt* component_;
};


#endif //TESTTXMESSAGE_H