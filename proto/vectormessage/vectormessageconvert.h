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


    template <class T> ::VectorMessage* decode(std::string message);
    std::string encode(::VectorMessage* message);


      
    template <> ::VectorMessage* decode <cdit::VectorMessage>(std::string message);

};

#endif //PROTO_VECTORMESSAGE_CONVERT_H
