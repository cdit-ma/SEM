#ifndef PB_TRANSLATE_H
#define PB_TRANSLATE_H

#include "../core/attribute.h"

namespace NodeManager{
    class Attribute;
};

::Attribute* SetAttributeFromPb(NodeManager::Attribute* attr, ::Attribute* a = 0);

#endif //ACTIVATABLE_H