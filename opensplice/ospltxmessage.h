#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H

//Include the core Elements
#include "core/globalinterfaces.hpp"
#include "core/eventports/outeventport.hpp"

//Include the concrete message object
#include "../message.h"
namespace ospl{
     ::OutEventPort<::Message>* construct_TxMessage(Component* component, 
                                                    int domain_id, 
                                                    std::string subscriber_name, 
                                                    std::string topic_name);
};

#endif //OSPLTXMESSAGE_H