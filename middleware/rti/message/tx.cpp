#include "tx.h"

#include <rti/outeventport.hpp>



::OutEventPort<::Message>* rti::Message::construct_tx(Component* component, std::string name){
    return new rti::OutEventPort<::Message, cdit::Message>(component, name);
}
