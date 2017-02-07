#include "rx.h"

#include <opensplice/ineventport.hpp>
#include "message_DCPS.hpp"


::InEventPort<::Message>* ospl::Message::ConstructRx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new ospl::InEventPort<::Message, cdit::Message>(component, name, callback_function);
}