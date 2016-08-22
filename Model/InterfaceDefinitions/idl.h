#ifndef IDL_H
#define IDL_H
#include "../node.h"

class IDL : public Node
{
    Q_OBJECT
public:
    IDL();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // IDL_H
