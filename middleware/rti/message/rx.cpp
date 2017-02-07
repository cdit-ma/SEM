#include "rx.h"

#include <rti/ineventport.hpp>
#include "message.hpp"


::InEventPort<::Message>* rti::Message::ConstructRx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new rti::InEventPort<::Message, cdit::Message>(component, name, callback_function);
}
