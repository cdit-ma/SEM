#ifndef TESTTXMESSAGE_H
#define TESTTXMESSAGE_H

#include "../interfaces.h"

class test_txMessage: public txMessageInt{
    void txMessage(Message* message);
};


#endif //TESTTXMESSAGE_H