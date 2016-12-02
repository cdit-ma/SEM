#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H

#include <dds/dds.hpp>

#include "../interfaces.h"
#include "message_DCPS.hpp"

namespace ospl{
    class TxMessage: public txMessageInt{
        public:
            TxMessage(txMessageInt* component, dds::pub::Publisher publisher, std::string topic_name);
            
            void txMessage(Message* message);
        private:
            txMessageInt* component_;

            dds::pub::Publisher publisher_ = dds::pub::Publisher(dds::core::null);
            dds::pub::DataWriter<test_dds::Message> writer_ = dds::pub::DataWriter<test_dds::Message>(dds::core::null);
    };
};

#endif //OSPLTXMESSAGE_