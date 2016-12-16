#ifndef ZMQRXMESSAGE_H
#define ZMQRXMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

namespace proto{
    class Message;
};

namespace zmq{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class Zmq_InEventPort;

    class RxMessage: public rxMessageInt{
        public:
            EXPORT_FUNC RxMessage(rxMessageInt* component, std::string end_point);
            void rxMessage(::Message* message);
            void rx_(::Message* message);
        private:
            //This is the concrete event_port
            Zmq_InEventPort<::Message, proto::Message> * event_port_;

            //This is the Component this port should call into
            rxMessageInt* component_;        
    };
};

#endif //ZMQRXMESSAGE_H