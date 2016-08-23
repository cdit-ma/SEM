#ifndef EVENTPORTIMPL_H
#define EVENTPORTIMPL_H
#include "behaviournode.h"

class EventPortImpl : public BehaviourNode
{
    Q_OBJECT
public:
    EventPortImpl(Node::NODE_KIND kind);

    bool isInPort();
    bool isOutPort();

    virtual bool canAdoptChild(Node* child);
    virtual bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // EVENTPORTIMPL_H
