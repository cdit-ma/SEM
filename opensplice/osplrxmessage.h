#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H

#include <thread>

#include "../interfaces.h"
#include "../message.h"

namespace test_dds{
    class Message;
};


namespace ospl{
    ::Message* translate(test_dds::Message m); 
    class RxMessage: public rxMessageInt{
        public:
            RxMessage(rxMessageInt* component, int domain_id, std::string  subscriber_name, std::string  reader_name, std::string  topic_name);
            void rxMessage(Message* message);
        private:
            void recieve();

            int domain_id;
            std::string subscriber_name;
            std::string reader_name;
            std::string topic_name;

            std::thread* rec_thread_;
            rxMessageInt* component_;
    };
};

#endif //OSPLRXMESSAGE_H