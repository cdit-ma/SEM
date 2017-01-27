#ifndef RTI_MESSAGE_CONVERT_H
#define RTI_MESSAGE_CONVERT_H

#include "../../datatypes/message/message.h"

namespace cdit{
    class Message;
};

namespace rti{
    cdit::Message* translate(::Message *m);
    ::Message* translate(cdit::Message *m);
};

#endif //RTI_MESSAGE_CONVERT_H
