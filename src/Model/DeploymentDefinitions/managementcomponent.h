#ifndef MANAGEMENTCOMPONENT_H
#define MANAGEMENTCOMPONENT_H
#include "../node.h"

class ManagementComponent : public Node
{
    Q_OBJECT
public:
    ManagementComponent();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MANAGEMENTCOMPONENT_H
