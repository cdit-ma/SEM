#ifndef ZMQTXMESSAGE_H
#define ZMQTXMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"


namespace proto{
    class Message;
};

namespace zmq{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class OutEventPort;

    class TxMessage: public txMessageInt{
        public:
            EXPORT_FUNC TxMessage(txMessageInt* component, std::string endpoint);
            void txMessage(::Message* message);
            void tx_(::Message* message){};
        private:
            //This is the concrete event port
            zmq::OutEventPort<::Message, proto::Message> * event_port_;

            //This is the Component this port should call into
            txMessageInt* component_;        
    };
};

#endif //ZMQTXMESSAGE_H