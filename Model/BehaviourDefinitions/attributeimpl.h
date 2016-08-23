#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../InterfaceDefinitions/datanode.h"

class AttributeImpl : public DataNode
{
    Q_OBJECT
public:
    AttributeImpl();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // ATTRIBUTEIMPL_H
