#ifndef RE_NODEMANAGER_TRANSLATE_H
#define RE_NODEMANAGER_TRANSLATE_H

#include "activatable.h"
#include "attribute.h"

namespace NodeManager{
    class Attribute;
};

std::weak_ptr<Attribute> SetAttributeFromPb(std::shared_ptr<Activatable> a, const NodeManager::Attribute& attr);
ATTRIBUTE_TYPE GetAttributeTypeFromPb(NodeManager::Attribute* attr);

#endif //RE_NODEMANAGER_TRANSLATE_H