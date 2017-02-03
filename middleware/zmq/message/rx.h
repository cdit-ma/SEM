#ifndef ZMQ_MESSAGE_RX_H
#define ZMQ_MESSAGE_RX_H

//Include the core Elements
#include <core/eventports/ineventport.hpp>

#include "../../proto/message/convert.h"

namespace zmq{
    namespace Message{
        ::InEventPort<::Message>* construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function);
    }
};

#endif //ZMQ_MESSAGE_RX_H