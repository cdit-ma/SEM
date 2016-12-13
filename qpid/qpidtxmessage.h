#ifndef QPIDTXMESSAGE_H
#define QPIDTXMESSAGE_H

#include "../interfaces.h"
#include "qpid/messaging/Connection.h"
#include "qpid/messaging/Message.h"
#include "qpid/messaging/Sender.h"
#include "qpid/messaging/Session.h"


class qpid_txMessage: public txMessageInt{
    public:
        qpid_txMessage(txMessageInt* component, qpid::messaging::Connection* connection, std::string endpoint);
        void txMessage(std::string message);
    private:
        txMessageInt* component_;
        qpid::messaging::Connection* connection_;
        qpid::messaging::Session session_;
        qpid::messaging::Sender sender_;
};


#endif //QPIDTXMESSAGE_H