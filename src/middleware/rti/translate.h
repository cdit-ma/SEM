#ifndef RTI_TRANSLATE_H
#define RTI_TRANSLATE_H

#include <core/globalinterfaces.hpp>

namespace rti{
    template <class BaseType, class RtiType>
    RtiType* EXPORT_FUNC translate(const BaseType& value);
    
    template <class BaseType, class RtiType>
    BaseType* EXPORT_FUNC translate(const RtiType& value);
};

#endif // RTI_TRANSLATE_H
