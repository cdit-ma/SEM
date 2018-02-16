#ifndef RTI_TRANSLATE_H
#define RTI_TRANSLATE_H

#include <core/globalinterfaces.hpp>

    template <class BaseType, class RtiType>
    RtiType* EXPORT_FUNC rti::translate(const BaseType& value);
    
    template <class BaseType, class RtiType>
    BaseType* EXPORT_FUNC rti::translate(const RtiType& value);

#endif // RTI_TRANSLATE_H
