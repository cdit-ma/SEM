#ifndef PB_TRANSLATE_H
#define PB_TRANSLATE_H

#include <core/attribute.h>

namespace NodeManager{
    class Attribute;
};

std::weak_ptr<Attribute> SetAttributeFromPb(std::shared_ptr<Activatable> a, const NodeManager::Attribute& attr);

ATTRIBUTE_TYPE GetAttributeTypeFromPb(NodeManager::Attribute* attr);


#endif //ACTIVATABLE_H