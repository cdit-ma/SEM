#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "behaviournode.h"

class AttributeImpl : public BehaviourNode
{
public:
    AttributeImpl();
    ~AttributeImpl();

public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // ATTRIBUTEIMPL_H
