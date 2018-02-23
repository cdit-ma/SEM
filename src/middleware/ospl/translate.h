#ifndef OSPL_TRANSLATE_H
#define OSPL_TRANSLATE_H

#include <core/globalinterfaces.hpp>

namespace ospl{
    template <class BaseType, class OsplType>
    EXPORT_FUNC OsplType* translate(const BaseType& value);
    
    template <class BaseType, class OsplType>
    EXPORT_FUNC BaseType* translate(const OsplType& value);
};

#endif // OSPL_TRANSLATE_H
