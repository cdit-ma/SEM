#include "tx.h"
#include <qpid/outeventport.hpp>

::OutEventPort<::Message>* qpid::construct_tx(Component* component, std::string name){
    return new qpid::OutEventPort<::Message, proto::Message>(component, name);
}
