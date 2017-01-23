#include "qpidrxmessage.h"
#include "../proto/message/messageconvert.h"
#include "qpid/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::Message>* qpid::construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    auto p = new qpid::InEventPort<::Message, proto::Message>(component, name, callback_function);
    return p;
}