#include "tx.h"

#include <opensplice/outeventport.hpp>
#include "message_DCPS.hpp"


::OutEventPort<::Message>* ospl::Message::ConstructTx(Component* component, std::string name){
    return new ospl::OutEventPort<::Message, cdit::Message>(component, name);
}