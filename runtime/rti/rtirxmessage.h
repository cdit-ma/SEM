#ifndef RTI_RXMESSAGE_H
#define RTI_RXMESSAGE_H

#define EXPORTED __attribute__((visibility("default")))

//Include the concrete port interfaces
#include "../interfaces.h"

//Includes the ::Message and rti::Message
#include "messageconvert.h"

namespace rti{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class InEventPort;

    class RxMessage: public rxMessageInt{
        public:
            EXPORTED RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name, std::string topic_name);
            void rxMessage(::Message* message);
            void rx_(::Message* message);
        private:
            //This is the concrete event port
            rti::InEventPort<::Message, cdit::Message> * event_port_;

            rxMessageInt* component_;
    };
};

#endif //RTI_RXMESSAGE_H
