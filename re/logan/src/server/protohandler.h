#ifndef SERVER_PROTOHANDLER_H
#define SERVER_PROTOHANDLER_H

namespace zmq{ class ProtoReceiver; }
// REVIEW(Jackson): This should be namespaced and moved to somewhere that makes more sense
class ProtoHandler{
    public:
        virtual ~ProtoHandler(){};
        virtual void BindCallbacks(zmq::ProtoReceiver& receiver) = 0;
};


#endif //SERVER_PROTOHANDLER_H
