#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "behaviournode.h"

class AttributeImpl : public BehaviourNode
{
public:
    AttributeImpl();
    ~AttributeImpl();

public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // ATTRIBUTEIMPL_H
