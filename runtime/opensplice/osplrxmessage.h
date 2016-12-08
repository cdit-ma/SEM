#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H

#include <thread>

#include "../interfaces.h"

//Includes the ::Message and ospl::Message
#include "messageconvert.h"



//Forward declare the AnyDataReader so that it can be linked without the <dds/dds.hpp> being linked.
namespace dds{
    namespace sub{
        class AnyDataReader;
    };
};

namespace ospl{ 
    class RxMessage: public rxMessageInt{
        public:
            RxMessage(rxMessageInt* component, int domain_id, std::string  subscriber_name, std::string  reader_name, std::string  topic_name);
            void rxMessage(::Message* message);
        private:
            void recieve();

            int domain_id;
            std::string subscriber_name;
            std::string reader_name;
            std::string topic_name;

            std::thread* rec_thread_;
            rxMessageInt* component_;
            dds::sub::AnyDataReader* reader_;
    };
};

#endif //OSPLRXMESSAGE_H