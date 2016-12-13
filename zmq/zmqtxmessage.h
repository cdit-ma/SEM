#ifndef ZMQTXMESSAGE_H
#define ZMQTXMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

//Includes the ::Message and proto::Message
#include "../proto/messageconvert.h"

namespace zmq{
    //Forward declare the Middleware specific EventPort
    template <class T> class OutEventPort;

    class TxMessage: public txMessageInt{
        public:
            TxMessage(txMessageInt* component, std::string endpoint);
            void txMessage(::Message* message);
            void tx_(::Message* message);
        private:
            //This is the concrete event port
            OutEventPort<::Message> * event_port_;

            //This is the Component this port should call into
            txMessageInt* component_;        
    };
};

#endif //ZMQTXMESSAGE_H