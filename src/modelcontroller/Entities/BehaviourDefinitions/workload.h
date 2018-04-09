#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "../node.h"

class EntityFactory;
class Workload: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Workload(EntityFactory* factory);
	Workload();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKLOAD_H
