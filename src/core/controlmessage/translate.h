#ifndef PB_TRANSLATE_H
#define PB_TRANSLATE_H

#include "../attribute.h"

namespace NodeManager{
    class Attribute;
};

::Attribute* SetAttributeFromPb(NodeManager::Attribute* attr, ::Attribute* a = 0);

ATTRIBUTE_TYPE GetAttributeTypeFromPb(NodeManager::Attribute* attr);


#endif //ACTIVATABLE_H