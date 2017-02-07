#include "tx.h"

#include <rti/outeventport.hpp>
#include "message.hpp"


::OutEventPort<::Message>* rti::Message::ConstructTx(Component* component, std::string name){
    return new rti::OutEventPort<::Message, cdit::Message>(component, name);
}
