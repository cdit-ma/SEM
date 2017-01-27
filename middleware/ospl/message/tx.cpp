#include "tx.h"

#include <opensplice/outeventport.hpp>

::OutEventPort<::Message>* ospl::Message::construct_tx(Component* component, std::string name){
    return new ospl::OutEventPort<::Message, cdit::Message>(component, name);
}