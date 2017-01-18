#include "ospltxmessage.h"

#include "message_DCPS.hpp"

#include "opensplice/outeventport.hpp"


EXPORT_FUNC ::OutEventPort<::Message>* ospl::construct_TxMessage(Component* component, 
                                                    int domain_id, 
                                                    std::string subscriber_name, 
                                                    std::string topic_name){

    auto p = new ospl::OutEventPort<::Message, cdit::Message>(component, domain_id,subscriber_name,topic_name);
    return p;
}
