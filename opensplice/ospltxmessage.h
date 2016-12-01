#ifndef DDSTXMESSAGE_H
#define DDSTXMESSAGE_H

#include "../interfaces.h"
#include "ndds/ndds_cpp.h"
#include "message.h"
#include "messageSupport.h"
#include "messagePlugin.h"


class dds_txMessage: public txMessageInt{
    public:
        dds_txMessage(txMessageInt* component, DDSPublisher* publisher, DDSTopic* topic);
        void txMessage(Message* message);
    private:
        txMessageInt* component_;
        DDSPublisher* publisher_;
        std::string topic_;
        test_dds::MessageDataWriter* writer_;
};


#endif //DDSTXMESSAGE_H