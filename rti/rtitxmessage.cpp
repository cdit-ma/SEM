#include "rtitxmessage.h"

#include "message.hpp"

#include "rti/outeventport.hpp"
#include "messageconvert.h"


EXPORT_FUNC ::OutEventPort<::Message>* rti::construct_TxMessage(Component* component, 
                                                    int domain_id, 
                                                    std::string subscriber_name, 
                                                    std::string topic_name){

    auto p = new rti::OutEventPort<::Message, proto::Message>(component, domain_id,subscriber_name,topic_name);
    return p;
}