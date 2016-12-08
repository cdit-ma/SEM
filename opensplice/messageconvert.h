#ifndef OSPLMESSAGECONVERT_H
#define OSPLMESSAGECONVERT_H

#include "../message.h"

namespace ospl{
    class Message;

    ospl::Message* translate(::Message *m);
    ::Message* translate(ospl::Message *m);
};

#endif //OSPLMESSAGECONVERT_H