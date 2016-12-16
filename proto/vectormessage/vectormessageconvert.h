#ifndef PROTO_VECTORMESSAGE_CONVERT_H
#define PROTO_VECTORMESSAGE_CONVERT_H

#include "../../vectormessage.h"

namespace cdit{
    class VectorMessage;
};

namespace proto{
    cdit::VectorMessage* translate(::VectorMessage* message);
    VectorMessage* translate(cdit::VectorMessage* message);
    //Special helpers


    ::VectorMessage* decode(std::string message, cdit::VectorMessage* m);
    std::string encode(::VectorMessage* message);
};

#endif //PROTO_VECTORMESSAGE_CONVERT_H
