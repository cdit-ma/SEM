#ifndef RTI_MESSAGE_TX_H
#define RTI_MESSAGE_TX_H

//Include the core Elements
#include <core/eventports/outeventport.hpp>

//Include the concrete message object
#include "convert.h"

namespace rti{
    namespace Message{
        ::OutEventPort<::Message>* ConstructTx(Component* component, std::string name);
    }
};


#endif //RTI_MESSAGE_TX_H