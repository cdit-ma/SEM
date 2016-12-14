#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H


//Include the concrete port interfaces
#include "../interfaces.h"

//Includes the ::Message and ospl::Message
#include "messageconvert.h"


namespace ospl{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class InEventPort;

    class RxMessage: public rxMessageInt{
        public:
            RxMessage(rxMessageInt* component, int domain_id, std::string  subscriber_name, std::string  reader_name, std::string topic_name);
            void rxMessage(::Message* message);
            void rx_(::Message* message);
        private:
            //This is the concrete event port
            ospl::InEventPort<::Message, cdit::Message> *event_port_;

            rxMessageInt* component_;
    };
};

#endif //OSPLRXMESSAGE_H
