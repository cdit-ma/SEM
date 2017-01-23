#include "rtirxmessage.h"

#include "message.hpp"

#include "messageconvert.h"

//Include the templated OutEventPort Implementation for OSPL
#include "rti/ineventport.hpp"



EXPORT_FUNC ::InEventPort<::Message>* rti::construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    auto p = new rti::InEventPort<::Message, cdit::Message>(component, name, callback_function);
    return p;
}