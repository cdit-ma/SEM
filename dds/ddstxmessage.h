#ifndef ZMQTXMESSAGE_H
#define ZMQTXMESSAGE_H

#include "../interfaces.h"
#include "ndds/ndds_cpp.h"


class dds_txMessage: public txMessageInt{
    public:
        dds_txMessage(txMessageInt* component, DDSPublisher* publisher);
        void txMessage(Message* message);
    private:
        txMessageInt* component_;
        DDSPublisher* publisher_;
        std::string topic_;
        test_dds::MessageDataWriter* writer_;
};


#endif //ZMQTXMESSAGE_H