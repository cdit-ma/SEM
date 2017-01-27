#include "rx.h"

#include <opensplice/ineventport.hpp>

::InEventPort<::Message>* ospl::Message::construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new ospl::InEventPort<::Message, cdit::Message>(component, name, callback_function);
}