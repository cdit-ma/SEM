#include "qpidtxmessage.h"
#include "../proto/message/messageconvert.h"
#include "qpid/outeventport.hpp"

EXPORT_FUNC ::OutEventPort<::Message>* qpid::construct_TxMessage(Component* component, std::string broker, std::string topic){

    auto p = new qpid::OutEventPort<::Message, proto::Message>(component, broker, topic);
    return p;
}
