#ifndef REPLYPORTIMPL_H
#define REPLYPORTIMPL_H
#include "behaviournode.h"

class EntityFactory;
class ReplyPortImpl : public BehaviourNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ReplyPortImpl(EntityFactory* factory);
	ReplyPortImpl();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REPLYPORTIMPL_H
