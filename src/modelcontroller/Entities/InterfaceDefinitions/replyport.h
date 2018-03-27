#ifndef REPLYPORT_H
#define REPLYPORT_H
#include "../node.h"

class EntityFactory;
class ReplyPort : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ReplyPort(EntityFactory* factory);
	ReplyPort();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REPLYPORT_H
