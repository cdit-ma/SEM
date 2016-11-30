#ifndef DDSRXMESSAGE_H
#define DDSRXMESSAGE_H

#include "../interfaces.h"
#include <ndds/ndds_cpp.h>


class dds_rxMessage: public rxMessageInt{
    public:
        dds_rxMessage(rxMessageInt* component, DDSSubscriber* subscriber);
        void rxMessage(Message* message);
    private:
        void recieve();


        std::thread* rec_thread_;
        rxMessageInt* component_;
        DDSSubscriber* subscriber_;
        test_dds::MessageDataReader* reader_;
};


#endif //DDSRXMESSAGE_H