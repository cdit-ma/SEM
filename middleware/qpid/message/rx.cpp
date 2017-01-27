#include "rx.h"
#include <qpid/ineventport.hpp>

::InEventPort<::Message>* qpid::construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new qpid::InEventPort<::Message, proto::Message>(component, name, callback_function);
}