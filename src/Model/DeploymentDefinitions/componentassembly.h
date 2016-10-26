#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "../node.h"

class ComponentAssembly : public Node
{
    Q_OBJECT
public:
    ComponentAssembly();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENTASSEMBLY_H
