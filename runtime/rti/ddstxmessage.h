#ifndef DDSTXMESSAGE_H
#define DDSTXMESSAGE_H

#include "../interfaces.h"

#include <dds/dds.hpp>
#include "message.hpp"


class dds_txMessage: public txMessageInt{
    public:
        dds_txMessage(txMessageInt* component, dds::pub::Publisher* publisher, dds::topic::Topic<test_dds::Message>* topic);
        void txMessage(Message* message);
    private:
        txMessageInt* component_;
        dds::pub::Publisher* publisher_;

        dds::pub::DataWriter<test_dds::Message>* writer_;
        dds::topic::Topic<test_dds::Message>* topic_;
};


#endif //DDSTXMESSAGE_H