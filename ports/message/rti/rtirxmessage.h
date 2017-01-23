#ifndef RTI_RXMESSAGE_H
#define RTI_RXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

//Include the concrete message object
#include "../message.h"

namespace rti{
    ::InEventPort<::Message>* construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function);
};

#endif //RTI_RXMESSAGE_H
