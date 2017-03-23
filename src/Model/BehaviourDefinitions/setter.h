#ifndef SETFUNCTION_H
#define SETFUNCTION_H
#include "behaviournode.h"

class Setter: public BehaviourNode
{
    Q_OBJECT
public:
    Setter();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif //SETFUNCTION_H
