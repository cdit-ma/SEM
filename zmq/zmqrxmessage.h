#ifndef ZMQRXMESSAGE_H
#define ZMQRXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"


//Include the concrete message object
#include "../message.h"

namespace zmq{
     ::InEventPort<::Message>* construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function);
};

#endif //ZMQRXMESSAGE_H