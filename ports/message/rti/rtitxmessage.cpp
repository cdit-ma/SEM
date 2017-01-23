#include "rtitxmessage.h"

#include "message.hpp"

#include "messageconvert.h"

#include "rti/outeventport.hpp"



EXPORT_FUNC ::OutEventPort<::Message>* rti::construct_TxMessage(Component* component, std::string name){
    auto p = new rti::OutEventPort<::Message, cdit::Message>(component, name);
    return p;
}
