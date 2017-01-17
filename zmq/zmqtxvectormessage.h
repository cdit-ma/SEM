#ifndef ZMQTXVECTORMESSAGE_H
#define ZMQTXVECTORMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/outeventport.hpp"

//Include the concrete message object
#include "../vectormessage.h"

namespace zmq{
     ::OutEventPort<::VectorMessage>* construct_TxVectorMessage(Component* component, std::string endpoint);
};

#endif //ZMQTXVECTORMESSAGE_H