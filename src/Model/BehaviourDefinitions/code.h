#ifndef MODEL_IMPL_CODE_H
#define MODEL_IMPL_CODE_H

#include "behaviournode.h"

class Code: public BehaviourNode
{
    Q_OBJECT
public:
    Code();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MODEL_IMPL_CODE_H
