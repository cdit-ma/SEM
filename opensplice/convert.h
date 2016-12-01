#ifndef OPENSPLICE_CONVERT_H
#define OPENSPLICE_CONVERT_H

#include "../message.h"
#include "message.h"

namespace opensplice{
    ospl::Message* message_to_opensplice(::Message* message);
    ::Message* opensplice_to_message(ospl::Message* message);
    
};

#endif // OPENSPLICE_CONVERT_H
