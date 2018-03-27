#ifndef REQUESTPORTIMPL_H
#define REQUESTPORTIMPL_H
#include "behaviournode.h"

class EntityFactory;
class RequestPortImpl : public BehaviourNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	RequestPortImpl(EntityFactory* factory);
	RequestPortImpl();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REQUESTPORTIMPL_H
