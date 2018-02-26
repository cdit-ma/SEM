#ifndef RTI_TRANSLATE_H
#define RTI_TRANSLATE_H

#include <core/globalinterfaces.hpp>

namespace rti{
    template <class BaseType, class RtiType>
    EXPORT_FUNC RtiType* translate(const BaseType& value);
    
    template <class BaseType, class RtiType>
    EXPORT_FUNC BaseType* translate(const RtiType& value);
};

#endif // RTI_TRANSLATE_H
