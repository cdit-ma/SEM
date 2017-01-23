#ifndef QPIDTXMESSAGE_H
#define QPIDTXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/outeventport.hpp"

//Include the concrete message object
#include "../message.h"

namespace qpid{
    ::OutEventPort<::Message>* construct_TxMessage(Component* component, std::string name);
};

#endif //QPIDTXMESSAGE_H