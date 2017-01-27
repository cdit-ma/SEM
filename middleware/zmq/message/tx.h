#ifndef ZMQ_MESSAGE_TX_H
#define ZMQ_MESSAGE_TX_H

//Include the core Elements
#include <core/globalinterfaces.hpp>
#include <core/eventports/outeventport.hpp>

#include "../../proto/message/convert.h"

namespace zmq{
    namespace Message{
        ::OutEventPort<::Message>* construct_tx(Component* component, std::string name);
    }
};

#endif //ZMQ_MESSAGE_TX_H