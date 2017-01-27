#ifndef QPID_MESSAGE_TX_H
#define QPID_MESSAGE_TX_H

//Include the core Elements
#include <core/globalinterfaces.hpp>
#include <core/eventports/outeventport.hpp>

#include "../../proto/message/convert.h"

namespace qpid{
    namespace Message{
        ::OutEventPort<::Message>* construct_tx(Component* component, std::string name);
    }
};

#endif //QPID_MESSAGE_TX_H