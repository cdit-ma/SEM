#ifndef ZMQRXMESSAGE_H
#define ZMQRXMESSAGE_H

#include "../interfaces.h"
#include "zmq.hpp"

#include <thread>

class zmq_rxMessage: public rxMessageInt{
    public:
        zmq_txMessage(txMessageInt* component, zmq::context_t* context, std::string endpoint);
        void rxMessage(Message* message);
    private:
        void recieve();

        std::thread* rec_thread_;
        txMessageInt* component_;
        zmq::context_t* context_;
        zmq::socket_t* socket_;
};


#endif //ZMQRXMESSAGE_H