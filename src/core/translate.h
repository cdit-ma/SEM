#ifndef RE_NODEMANAGER_TRANSLATE_H
#define RE_NODEMANAGER_TRANSLATE_H

#include "activatable.h"
#include "attribute.h"

namespace NodeManager{
    class Attribute;
};

std::weak_ptr<Attribute> SetAttributeFromPb(Activatable& a, const NodeManager::Attribute& attr);
ATTRIBUTE_TYPE GetAttributeTypeFromPb(const NodeManager::Attribute& attr);

#endif //RE_NODEMANAGER_TRANSLATE_H