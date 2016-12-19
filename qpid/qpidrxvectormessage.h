#ifndef QPIDRXVECTORMESSAGE_H
#define QPIDRXVECTORMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

namespace cdit{
    class VectorMessage;
};

namespace qpid{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class InEventPort;

    class RxVectorMessage: public rxVectorMessageInt{
        public:
            EXPORT_FUNC RxVectorMessage(rxVectorMessageInt* component, std::string broker, std::string topic);
            void rxVectorMessage(::VectorMessage* message);
            void rx_(::VectorMessage* message);
        private:
            //This is the concrete event_port
            qpid::InEventPort<::VectorMessage, cdit::VectorMessage> * event_port_;

            //This is the Component this port should call into
            rxVectorMessageInt* component_;        
    };
};

#endif //QPIDRXVECTORMESSAGE_H