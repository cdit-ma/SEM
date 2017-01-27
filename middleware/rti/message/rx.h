#ifndef RTI_MESSAGE_RX_H
#define RTI_MESSAGE_RX_H

//Include the core Elements
#include <core/globalinterfaces.hpp>
#include <core/eventports/ineventport.hpp>

#include "convert.h"

namespace rti{
    namespace Message{
        ::InEventPort<::Message>* construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function);
    }
};

#endif //RTI_MESSAGE_RX_H
