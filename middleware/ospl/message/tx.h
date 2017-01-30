#ifndef OSPL_MESSAGE_TX_H
#define OSPL_MESSAGE_TX_H

//Include the core Elements
#include <core/globalinterfaces.hpp>
#include <core/eventports/outeventport.hpp>

#include "convert.h"

namespace ospl{
    namespace Message{
        ::OutEventPort<::Message>* construct_tx(Component* component, std::string name);
    }
};

#endif //OSPL_MESSAGE_TX_H