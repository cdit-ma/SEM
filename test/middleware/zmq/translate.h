#ifndef PB_TRANSLATE_H
#define PB_TRANSLATE_H

#include <core/attribute.h>

namespace NodeManager{
    class Attribute;
};

std::shared_ptr<Attribute> SetAttributeFromPb(NodeManager::Attribute* attr, std::shared_ptr<Attribute> a = 0);

ATTRIBUTE_TYPE GetAttributeTypeFromPb(NodeManager::Attribute* attr);


#endif //ACTIVATABLE_H