#ifndef INTERFACEDEFINITIONS_H
#define INTERFACEDEFINITIONS_H
#include "../node.h"

class InterfaceDefinitions: public Node
{
    Q_OBJECT
public:
    InterfaceDefinitions();
    VIEW_ASPECT getViewAspect();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // INTERFACEDEFINITIONS_H
