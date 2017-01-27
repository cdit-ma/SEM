#ifndef PROTO_VECTORMESSAGE_CONVERT_H
#define PROTO_VECTORMESSAGE_CONVERT_H

#include "../../datatypes/message/message.h"

namespace cdit{
    class VectorMessage;
};

namespace proto{
    cdit::VectorMessage* translate(const ::VectorMessage* message);
    ::VectorMessage* translate(const cdit::VectorMessage* message);
    
    //Special helpers
    template <class T> ::VectorMessage* decode(const std::string message);
    std::string encode(const ::VectorMessage* message);

    //Forward Declare this function
    template <> ::VectorMessage* decode <cdit::VectorMessage>(std::string message);
};

#endif //PROTO_VECTORMESSAGE_CONVERT_H
