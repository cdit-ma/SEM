#include "osplrxmessage.h"

#include "message_DCPS.hpp"

#include "messageconvert.h"

//Include the templated OutEventPort Implementation for OSPL
#include "opensplice/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::Message>* ospl::construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    auto p = new ospl::InEventPort<::Message, cdit::Message>(component, name, callback_function);
    return p;
}