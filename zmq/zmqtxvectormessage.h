#ifndef ZMQTXVECTORMESSAGE_H
#define ZMQTXVECTORMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

namespace cdit{
    class VectorMessage;
}

namespace zmq{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class Zmq_OutEventPort;

    class TxVectorMessage: public txVectorMessageInt{
        public:
            EXPORT_FUNC TxVectorMessage(txVectorMessageInt* component, std::string endpoint);
            void txVectorMessage(::VectorMessage* message);
            void tx_(::VectorMessage* message){};
        private:
            //This is the concrete event port
            Zmq_OutEventPort<::VectorMessage, cdit::VectorMessage> * event_port_;

            //This is the Component this port should call into
            txVectorMessageInt* component_;        
    };
};

#endif //ZMQTXVECTORMESSAGE_H