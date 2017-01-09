#ifndef ZMQRXVECTORMESSAGE_H
#define ZMQRXVECTORMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

namespace cdit{
    class VectorMessage;
};

namespace zmq{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class InEventPort;

    class RxVectorMessage: public rxVectorMessageInt{
        public:
            EXPORT_FUNC RxVectorMessage(rxVectorMessageInt* component, std::string end_point);
            void rxVectorMessage(::VectorMessage* message);
            void rx_(::VectorMessage* message);
             void activate();
            void passivate();
            bool is_active();
        private:
            //This is the concrete event_port
            zmq::InEventPort<::VectorMessage, cdit::VectorMessage> * event_port_;

            //This is the Component this port should call into
            rxVectorMessageInt* component_;        
    };
};

#endif //ZMQRXVECTORMESSAGE_H