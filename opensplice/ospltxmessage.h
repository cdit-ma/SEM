#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H

#include "../interfaces.h"
#include "../message.h"

namespace ospl{
    class Message;
};

namespace ospl{
    ospl::Message translate(::Message *m);

    class TxMessage: public txMessageInt{
        public:
            TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name);
            
            void txMessage(::Message* message);
        private:
            txMessageInt* component_;
            
            int domain_id;
            std::string publisher_name;
            std::string writer_name;
            std::string topic_name;

    };
};

#endif //OSPLTXMESSAGE_H