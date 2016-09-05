#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class HardwareNode : public Node
{
    Q_OBJECT
public:
    HardwareNode();
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // HARDWARENODE_H
