#ifndef TESTRXMESSAGE_H
#define TESTRXMESSAGE_H

#include "../interfaces.h"

class test_rxMessage: public rxMessageInt{
    void rxMessage(Message* message);
};


#endif //TESTRXMESSAGE_H