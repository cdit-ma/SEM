#ifndef QPIDTXMESSAGE_H
#define QPIDTXMESSAGE_H

#include "../interfaces.h"
#include "../message.h"

class qpid_txMessage: public txMessageInt{
    public:
        qpid_txMessage(txMessageInt* component, std::string broker, std::string topic);
        void txMessage(Message* message);
    private:
        txMessageInt* component_;
        std::string broker_;

        std::string topic_;
};


#endif //QPIDTXMESSAGE_H