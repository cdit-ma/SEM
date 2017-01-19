#include "ospltxmessage.h"

#include "message_DCPS.hpp"
#include "messageconvert.h"
#include "opensplice/outeventport.hpp"


EXPORT_FUNC ::OutEventPort<::Message>* ospl::construct_TxMessage(Component* component, std::string name){
    auto p = new ospl::OutEventPort<::Message, cdit::Message>(component, name);
    return p;
}