#ifndef REQUESTPORTINSTANCE_H
#define REQUESTPORTINSTANCE_H
#include "../node.h"

class EntityFactory;
class RequestPortInstance : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	RequestPortInstance(EntityFactory* factory);
	RequestPortInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // REQUESTPORTINSTANCE_H
