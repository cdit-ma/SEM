#ifndef DDSTXMESSAGE_H
#define DDSTXMESSAGE_H

#include <dds/dds.hpp>

#include "../interfaces.h"
#include "message.hpp"

namespace rti{
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

#endif //DDSTXMESSAGE_H