#ifndef INTERFACEDEFINITIONS_H
#define INTERFACEDEFINITIONS_H
#include "../node.h"

class InterfaceDefinitions: public Node
{
    Q_OBJECT
public:
    InterfaceDefinitions();
    VIEW_ASPECT getViewAspect() const;

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // INTERFACEDEFINITIONS_H
