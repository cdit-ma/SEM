#ifndef PB_TRANSLATE_H
#define PB_TRANSLATE_H

#include "../core/attribute.h"

namespace NodeManager{
    class Attribute;
};

::Attribute* get_attribute_from_pb(NodeManager::Attribute* attr);

#endif //ACTIVATABLE_H