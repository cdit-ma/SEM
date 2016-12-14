#ifndef RTI_MESSAGECONVERT_H
#define RTI_MESSAGECONVERT_H

#include "../message.h"

namespace cdit{
    class Message;
}
namespace rti{
    cdit::Message* translate(::Message *m);
    ::Message* translate(const cdit::Message *m);
};

#endif //RTI_MESSAGECONVERT_H
