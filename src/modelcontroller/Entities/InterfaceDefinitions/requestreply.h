#ifndef REQUESTREPLY_H
#define REQUESTREPLY_H
#include "../node.h"

class EntityFactory;
class RequestReply : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	RequestReply(EntityFactory* factory);
	RequestReply();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REQUESTREPLY_H
