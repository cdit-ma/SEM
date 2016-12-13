#ifndef RTI_MESSAGECONVERT_H
#define RTI_MESSAGECONVERT_H

#include "../message.h"

namespace rti{
    class Message;

    rti::Message* translate(::Message *m);
    ::Message* translate(const rti::Message *m);
};

#endif //RTI_MESSAGECONVERT_H
