#ifndef QPIDRXVECTORMESSAGE_H
#define QPIDRXVECTORMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

//Include the concrete message object
#include "../vectormessage.h"

namespace qpid{
     ::InEventPort<::VectorMessage>* construct_RxVectorMessage(Component* component, std::function<void (::VectorMessage*)> callback_function, std::string broker, std::string topic);
};

#endif //QPIDRXVECTORMESSAGE_H