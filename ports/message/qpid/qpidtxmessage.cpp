#include "qpidtxmessage.h"
#include "../proto/message/messageconvert.h"
#include "qpid/outeventport.hpp"

EXPORT_FUNC ::OutEventPort<::Message>* qpid::construct_TxMessage(Component* component, std::string name){

    auto p = new qpid::OutEventPort<::Message, proto::Message>(component, name);
    return p;
}
