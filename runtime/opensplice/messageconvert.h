#ifndef OSPL_MESSAGECONVERT_H
#define OSPL_MESSAGECONVERT_H

#include "../message.h"


namespace cdit{
    class Message;
}

namespace ospl{
    
    
    cdit::Message* translate(::Message *m);
    ::Message* translate(const cdit::Message *m);
};

#endif //OSPL_MESSAGECONVERT_H
