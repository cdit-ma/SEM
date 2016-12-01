#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H

#include "../interfaces.h"
#include <thread>
#include <dds/core/ddscore.hpp>

#include "message.h"
#include "messageDcps.h"
//#include "messageSplDcps.h"

class ospl_rxMessage: public rxMessageInt{
    public:
        ospl_rxMessage(rxMessageInt* component, DDSSubscriber* subscriber, DDSTopic* topic);
        void rxMessage(Message* message);
    private:
        void recieve();


        std::thread* rec_thread_;
        rxMessageInt* component_;
        DDSSubscriber* subscriber_;
        test_ospl::MessageDataReader* reader_;
        DDSTopic* topic_;
};


#endif //osplRXMESSAGE_H