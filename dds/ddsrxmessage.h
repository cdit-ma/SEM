#ifndef DDSRXMESSAGE_H
#define DDSRXMESSAGE_H

#include "../interfaces.h"
#include <thread>
#include "ndds/ndds_cpp.h"

#include "message.h"
#include "messageSupport.h"
#include "messagePlugin.h"

class dds_rxMessage: public rxMessageInt{
    public:
        dds_rxMessage(rxMessageInt* component, DDSSubscriber* subscriber, DDSTopic* topic);
        void rxMessage(Message* message);
    private:
        void recieve();


        std::thread* rec_thread_;
        rxMessageInt* component_;
        DDSSubscriber* subscriber_;
        test_dds::MessageDataReader* reader_;
        DDSTopic* topic_;
};


#endif //DDSRXMESSAGE_H