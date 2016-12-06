#ifndef DDSTXMESSAGE_H
#define DDSTXMESSAGE_H

#include "../interfaces.h"
#include "../message.h"

namespace rti{
    class Message;
};

namespace rti{
    rti::Message translate(::Message *m);

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

#endif //DDSTXMESSAGE_H