#ifndef QPIDRXMESSAGE_H
#define QPIDRXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

#include "../message.h"

namespace qpid{
    ::InEventPort<::Message>* construct_RxMessage(Component* component, std::function<void (::Message*)> callback_function, std::string broker, std::string topic);
};

#endif //QPIDRXMESSAGE_H