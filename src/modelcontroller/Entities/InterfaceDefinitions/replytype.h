#ifndef REPLYTYPE_H
#define REPLYTYPE_H
#include "eventport.h"

class EntityFactory;
class ReplyType : public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ReplyType(EntityFactory* factory);
	ReplyType();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REPLYTYPE_H
