#ifndef TESTRXMESSAGE_H
#define TESTRXMESSAGE_H

#include "../interfaces.h"

class test_rxMessage: public rxMessageInt{

    public:
        test_rxMessage(rxMessageInt* component);
        void rxMessage(Message* message);
    private:
        rxMessageInt* component_;

};


#endif //TESTRXMESSAGE_H