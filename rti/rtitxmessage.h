#ifndef DDSTXMESSAGE_H
#define DDSTXMESSAGE_H

#include <dds/dds.hpp>

#include "ddstxinterface.h"

#include "../interfaces.h"
#include "message.hpp"





test_dds::Message convert_message(::Message *m);
namespace rti{
    class TxMessage: public txMessageInt{
        public:
            TxMessage(txMessageInt* component, dds::pub::Publisher publisher, std::string topic_name);
            
            void txMessage(Message* message);
        private:
            txMessageInt* component_;

            DDS_TX_Interface<test_dds::Message, ::Message>* tx_;
    };
};

#endif //DDSTXMESSAGE_H