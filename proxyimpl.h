#ifndef PROXYIMPL_H
#define PROXYIMPL_H

#include "interfaces.h"

class ProxyImpl: public ProxyInt{
    public:
        ProxyImpl(std::string name);
        void rxMessage(::Message* message);
};

#endif //PROXYIMPL_H
