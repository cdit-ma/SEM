#ifndef RTI_TXMESSAGE_H
#define RTI_TXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/outeventport.hpp"

//Include the concrete message object
#include "../message.h"

namespace rti{
     ::OutEventPort<::Message>* construct_TxMessage(Component* component, std::string name);
};

#endif //RTI_TXMESSAGE_H