#ifndef QPIDTXVECTORMESSAGE_H
#define QPIDTXVECTORMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

namespace cdit{
    class VectorMessage;
}

namespace qpid{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class OutEventPort;

    class TxVectorMessage: public txVectorMessageInt{
        public:
            EXPORT_FUNC TxVectorMessage(txVectorMessageInt* component, std::string endpoint);
            void txVectorMessage(::VectorMessage* message);
            void tx_(::VectorMessage* message){};
        private:
            //This is the concrete event port
            OutEventPort<::VectorMessage, cdit::VectorMessage> * event_port_;

            //This is the Component this port should call into
            txVectorMessageInt* component_;        
    };
};

#endif //QPIDTXVECTORMESSAGE_H