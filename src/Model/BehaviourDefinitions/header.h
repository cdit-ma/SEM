#ifndef MODEL_IMPL_HEADER_H
#define MODEL_IMPL_HEADER_H

#include "behaviournode.h"

class Header: public BehaviourNode
{
    Q_OBJECT
public:
    Header();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MODEL_IMPL_HEADER_H
