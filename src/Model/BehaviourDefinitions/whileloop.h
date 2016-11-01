#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "branch.h"

class WhileLoop: public Branch
{
    Q_OBJECT
public:
    WhileLoop();

    bool canAdoptChild(Node *node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // WHILELOOP_H
