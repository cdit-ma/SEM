#include "tx.h"
#include <qpid/outeventport.hpp>

::OutEventPort<::Message>* qpid::Message::ConstructTx(Component* component, std::string name){
    return new qpid::OutEventPort<::Message, proto::Message>(component, name);
}
