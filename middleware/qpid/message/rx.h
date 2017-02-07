#ifndef QPID_MESSAGE_RX_H
#define QPID_MESSAGE_RX_H

//Include the core Elements
#include <core/globalinterfaces.hpp>
#include <core/eventports/ineventport.hpp>

#include "../../proto/message/convert.h"

namespace qpid{
    namespace Message{
        ::InEventPort<::Message>* ConstructRx(Component* component, std::string name, std::function<void (::Message*)> callback_function);
    };
};

#endif //QPID_MESSAGE_RX_H