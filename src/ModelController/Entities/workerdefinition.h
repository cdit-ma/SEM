#ifndef WORKERDEFINITION_H
#define WORKERDEFINITION_H

#include "node.h"

class EntityFactory;
class WorkerDefinition: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	WorkerDefinition(EntityFactory* factory);
	WorkerDefinition();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKERDEFINITION_H
