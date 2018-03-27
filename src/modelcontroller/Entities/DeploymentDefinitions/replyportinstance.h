#ifndef REPLYPORTINSTANCE_H
#define REPLYPORTINSTANCE_H
#include "../node.h"

class EntityFactory;
class ReplyPortInstance : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ReplyPortInstance(EntityFactory* factory);
	ReplyPortInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REPLYPORTINSTANCE_H
