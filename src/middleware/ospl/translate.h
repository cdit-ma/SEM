#ifndef OSPL_TRANSLATE_H
#define OSPL_TRANSLATE_H

#include <core/globalinterfaces.hpp>

namespace rti{
    template <class BaseType, class OsplType>
    ProtoType* EXPORT_FUNC translate(const BaseType& value);
    
    template <class BaseType, class OsplType>
    BaseType* EXPORT_FUNC translate(const ProtoType& value);
};

#endif // OSPL_TRANSLATE_H
