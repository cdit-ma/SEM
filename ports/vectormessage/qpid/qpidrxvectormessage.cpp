#include "qpidrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated InEventPort Implementation for qpid
#include "qpid/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::VectorMessage>* qpid::construct_RxVectorMessage(Component* component, std::function<void (::VectorMessage*)> callback_function, std::string broker, std::string topic){
    auto p = new qpid::InEventPort<::VectorMessage, cdit::VectorMessage>(component, callback_function, broker, topic);
    return p;
}