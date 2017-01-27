#ifndef OSPL_MESSAGE_CONVERT_H
#define OSPL_MESSAGE_CONVERT_H

#include "../../datatypes/message/message.h"

namespace cdit{
    class Message;
};

namespace ospl{
    cdit::Message* translate(::Message *m);
    ::Message* translate(cdit::Message *m);
};

#endif //OSPL_MESSAGE_CONVERT_H

