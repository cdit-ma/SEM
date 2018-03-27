#ifndef REQUESTPORT_H
#define REQUESTPORT_H
#include "../node.h"

class EntityFactory;
class RequestPort : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	RequestPort(EntityFactory* factory);
	RequestPort();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REQUESTPORT_H
