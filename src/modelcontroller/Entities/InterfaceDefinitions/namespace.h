#ifndef NAMESPACE_H
#define NAMESPACE_H
#include "../node.h"

class EntityFactory;
class Namespace : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Namespace(EntityFactory* factory);
	Namespace();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // NAMESPACE_H
