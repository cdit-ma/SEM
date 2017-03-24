#ifndef FORCONDITION_H
#define FORCONDITION_H
#include "condition.h"

class ForCondition: public Condition
{
    Q_OBJECT
public:
    ForCondition();

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // FORCONDITION_H
