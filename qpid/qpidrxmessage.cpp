#include "qpidrxmessage.h"
#include "../proto/message/messageconvert.h"
#include "qpid/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::Message>* qpid::construct_RxMessage(Component* component, std::function<void (::Message*)> callback_function, std::string broker, std::string topic){
    auto p = new qpid::InEventPort<::Message, proto::Message>(component, callback_function, broker, topic);
    return p;
}