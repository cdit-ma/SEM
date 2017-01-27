#include "qpidtxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for qpid
#include "qpid/outeventport.hpp"

EXPORT_FUNC ::OutEventPort<::VectorMessage>* qpid::construct_TxVectorMessage(Component* component, std::string name){
    auto p = new qpid::OutEventPort<::VectorMessage, cdit::VectorMessage>(component, name);
    return p;
}