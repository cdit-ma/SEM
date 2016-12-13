#ifndef QPIDRXMESSAGE_H
#define QPIDRXMESSAGE_H

#include "../interfaces.h"
#include <thread>


class qpid_rxMessage: public rxMessageInt{
    public:
        qpid_rxMessage(rxMessageInt* component, std::string broker, std::string topic);
        void rxMessage(Message* message);
    private:
        void recieve();

        std::thread* rec_thread_;
        rxMessageInt* component_;
        std::string broker_;

        std::string topic_;
};


#endif //QPIDRXMESSAGE_H