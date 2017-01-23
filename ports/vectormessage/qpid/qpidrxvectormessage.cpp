#include "qpidrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated InEventPort Implementation for qpid
#include "qpid/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::VectorMessage>* qpid::construct_RxVectorMessage(Component* component, std::string name, std::function<void (::VectorMessage*)> callback_function){
    auto p = new qpid::InEventPort<::VectorMessage, cdit::VectorMessage>(component, name, callback_function);
    return p;
}