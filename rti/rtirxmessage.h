#ifndef RTI_RXMESSAGE_H
#define RTI_RXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/ineventport.hpp"

//Include the concrete message object
#include "../message.h"

namespace rti{
    ::InEventPort<::Message>* construct_RxMessage(Component* component, 
                                                    std::function<void (::Message*)> callback_function, 
                                                    int domain_id, 
                                                    std::string subscriber_name, 
                                                    std::string topic_name);
};

#endif //RTI_RXMESSAGE_H
