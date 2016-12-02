#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H

#include "../interfaces.h"

#include <dds/dds.hpp>

#include "message_DCPS.hpp"


class ospl_txMessage: public txMessageInt{
    public:
        ospl_txMessage(txMessageInt* component, dds::pub::Publisher publisher, std::string topic_name);
        
        void txMessage(Message* message);
    private:
        txMessageInt* component_;

        dds::pub::Publisher publisher_;
        dds::pub::DataWriter<test_dds::Message>  writer_;
};



#endif //OSPLTXMESSAGE_