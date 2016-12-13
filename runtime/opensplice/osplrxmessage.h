#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H

#include <thread>
#include <mutex>
#include <condition_variable>

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
            RxMessage(rxMessageInt* component, int domain_id, std::string  subscriber_name, std::string  reader_name, std::string topic_name);
            void rxMessage(::Message* message);
        private:
            void notify();
            void recieve_loop();
            
            std::thread* rec_thread_;
            
            std::mutex notify_mutex_;
            std::condition_variable notify_lock_condition_;
            
            rxMessageInt* component_;
            dds::sub::AnyDataReader* reader_;
    };
};

#endif //OSPLRXMESSAGE_H
