#ifndef EVENTPORTIMPL_H
#define EVENTPORTIMPL_H
#include "behaviournode.h"

class EntityFactory;
class EventPortImpl : public BehaviourNode
{
    Q_OBJECT
protected:
	EventPortImpl(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    
    EventPortImpl(NODE_KIND kind);
public:

    bool isInPort();
    bool isOutPort();

    virtual bool canAdoptChild(Node* child);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // EVENTPORTIMPL_H
