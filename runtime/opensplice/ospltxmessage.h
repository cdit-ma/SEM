#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H

#include "../interfaces.h"

//Includes the ::Message and ospl::Message
#include "messageconvert.h"

//Forward declare the AnyDataWriter so that it can be linked without the <dds/dds.hpp> being linked.
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