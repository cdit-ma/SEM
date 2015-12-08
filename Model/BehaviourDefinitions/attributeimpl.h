#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "behaviournode.h"

class AttributeImpl : public BehaviourNode
{
    Q_OBJECT
public:
    AttributeImpl();
    ~AttributeImpl();

public:
    bool canAdoptChild(Node* child);

    // Node interface
public:
    bool canConnect_DataEdge(Node *node);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // ATTRIBUTEIMPL_H
