#ifndef ZMQRXVECTORMESSAGE_H
#define ZMQRXVECTORMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

//Include the concrete message object
#include "../vectormessage.h"

namespace zmq{
    ::InEventPort<::VectorMessage>* construct_RxVectorMessage(Component* component, std::string name, std::function<void (::VectorMessage*)> callback_function);
};

#endif //ZMQRXVECTORMESSAGE_H