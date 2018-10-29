#ifndef SERVER_PROTOHANDLER_H
#define SERVER_PROTOHANDLER_H

namespace zmq{ class ProtoReceiver; }
class ProtoHandler{
    public:
        virtual ~ProtoHandler();
        virtual void BindCallbacks(zmq::ProtoReceiver& receiver) = 0;
};


#endif //SERVER_PROTOHANDLER_H
