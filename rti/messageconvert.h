#ifndef RTI_MESSAGECONVERT_H
#define RTI_MESSAGECONVERT_H

#include "../message.h"



namespace rti{
    namespace cdit{
    class Message;
}
    rti::cdit::Message* translate(::Message *m);
    ::Message* translate(const rti::cdit::Message *m);
};

#endif //RTI_MESSAGECONVERT_H
