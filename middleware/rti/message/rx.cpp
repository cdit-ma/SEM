#include "rx.h"

#include <rti/ineventport.hpp>

::InEventPort<::Message>* rti::Message::construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new rti::InEventPort<::Message, cdit::Message>(component, name, callback_function);
}