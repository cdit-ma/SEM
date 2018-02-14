#ifndef PROTO_CONVERT_H
#define PROTO_CONVERT_H

#include <string>
#include <core/globalinterfaces.hpp>

namespace proto{
    template <class BaseType, class ProtoType>
    ProtoType* EXPORT_FUNC translate(const BaseType& value);
    
    template <class BaseType, class ProtoType>
    BaseType* EXPORT_FUNC translate(const ProtoType& value);
    
    template <class BaseType, class ProtoType>
    BaseType* EXPORT_FUNC decode(const std::string& value);

    template <class BaseType, class ProtoType>
    std::string EXPORT_FUNC encode(const BaseType& value);
};

#endif // PROTO_MESSAGE_CONVERT_H
