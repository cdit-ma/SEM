#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H


#include "../interfaces.h"

#include "messageconvert.h"

namespace dds{
    namespace pub{
        class AnyDataWriter;
    };
};

namespace ospl{
    class TxMessage: public txMessageInt{
        public:
            TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name);
            void txMessage(::Message* message);
        private:
            dds::pub::AnyDataWriter* writer_;
            txMessageInt* component_;
    };
};

#endif //OSPLTXMESSAGE_H