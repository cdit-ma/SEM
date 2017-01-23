#ifndef ZMQTXMESSAGE_H
#define ZMQTXMESSAGE_H

//Include the core Elements
#include <core/globalinterfaces.hpp>
#include <core/eventports/outeventport.hpp>

//Include the concrete message object
#include "../message.h"

namespace zmq{
     ::OutEventPort<::Message>* construct_TxMessage(Component* component, std::string name);
};

#endif //ZMQTXMESSAGE_H