#ifndef OSPLRXMESSAGE_H
#define OSPLRXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

//Include the concrete message object
#include "../message.h"

namespace ospl{
    ::InEventPort<::Message>* construct_RxMessage(Component* component, 
                                                    std::function<void (::Message*)> callback_function, 
                                                    int domain_id, 
                                                    std::string subscriber_name, 
                                                    std::string topic_name);
};

#endif //OSPLRXMESSAGE_H
