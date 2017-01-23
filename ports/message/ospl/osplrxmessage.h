#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

//Include the concrete message object
#include "../message.h"

namespace ospl{
    ::InEventPort<::Message>* construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function);
};

#endif //OSPLRXMESSAGE_H
