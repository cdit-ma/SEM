#ifndef QPIDTXVECTORMESSAGE_H
#define QPIDTXVECTORMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/outeventport.hpp"

//Include the concrete message object
#include "../vectormessage.h"

namespace qpid{
     ::OutEventPort<::VectorMessage>* construct_TxVectorMessage(Component* component, std::string name);
};

#endif //QPIDTXVECTORMESSAGE_H