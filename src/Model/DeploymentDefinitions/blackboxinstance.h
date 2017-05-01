#ifndef BLACKBOXINSTANCE_H
#define BLACKBOXINSTANCE_H
#include "../node.h"

class EntityFactory;
class BlackBoxInstance: public Node
{
    Q_OBJECT
protected:
	BlackBoxInstance(EntityFactory* factory);
	BlackBoxInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // BLACKBOXINSTANCE_H

