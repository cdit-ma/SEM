#ifndef BLACKBOXINSTANCE_H
#define BLACKBOXINSTANCE_H
#include "../node.h"

class EntityFactory;
class BlackBoxInstance: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	BlackBoxInstance(EntityFactory* factory);
	BlackBoxInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // BLACKBOXINSTANCE_H

