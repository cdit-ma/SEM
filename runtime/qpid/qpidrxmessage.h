#ifndef QPIDRXMESSAGE_H
#define QPIDRXMESSAGE_H

#include "../interfaces.h"
#include "../proto/messageconvert.h"

namespace qpid{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class InEventPort;

    class RxMessage: public rxMessageInt{
        public:
            RxMessage(rxMessageInt* component, std::string broker, std::string topic);
            void rxMessage(::Message* message);
            void rx_(::Message* message);
        private:
            //This is the concrete event_port
            qpid::InEventPort<::Message, proto::Message> * event_port_;

            //This is the Component this port should call into
            rxMessageInt* component_;        
    };
};

#endif //QPIDRXMESSAGE_H