#ifndef QPIDTXMESSAGE_H
#define QPIDTXMESSAGE_H

#include "../interfaces.h"
#include "../proto/messageconvert.h"

namespace qpid{
    template <class T, class S> class OutEventPort;
    class TxMessage: public txMessageInt{
        public:
            EXPORT_FUNC TxMessage(txMessageInt* component, std::string broker, std::string topic);
            void txMessage(::Message* message);
            void tx_(::Message* message){};
        private:
            qpid::OutEventPort<::Message, proto::Message> * event_port_;

            txMessageInt* component_;
    };
};

#endif //QPIDTXMESSAGE_H