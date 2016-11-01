#ifndef MEMBER_H
#define MEMBER_H
#include "../node.h"

class Member : public Node
{
    Q_OBJECT
public:
    Member();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MEMBER_H
