#ifndef ZMQRXMESSAGE_H
#define ZMQRXMESSAGE_H

#include "../interfaces.h"
#include "zmq.hpp"
#include "convert.h"
#include <thread>

class zmq_rxMessage: public rxMessageInt{
    public:
        zmq_rxMessage(rxMessageInt* component, zmq::context_t* context, std::string endpoint);
        void rxMessage(Message* message);
    private:
        void recieve();

        std::thread* rec_thread_;
        rxMessageInt* component_;
        zmq::context_t* context_;
        zmq::socket_t* socket_;
        std::string endpoint_;
};


#endif //ZMQRXMESSAGE_H