#ifndef ZMQTXMESSAGE_H
#define ZMQTXMESSAGE_H

#include "../interfaces.h"
#include "zmq.hpp"

#include "convert.h"

class zmq_txMessage: public txMessageInt{
    public:
        zmq_txMessage(txMessageInt* component, zmq::context_t* context, std::string endpoint);
        void txMessage(Message* message);
    private:
        txMessageInt* component_;
        zmq::context_t* context_;
        zmq::socket_t* socket_;
};


#endif //ZMQTXMESSAGE_H