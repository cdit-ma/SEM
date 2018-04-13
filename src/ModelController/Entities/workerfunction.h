#ifndef WORKERFUNCTION_H
#define WORKERFUNCTION_H

#include "node.h"

class EntityFactory;
class WorkerFunction: public Node
{
public:
	WorkerFunction(EntityFactory* factory);
	WorkerFunction();

    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKERFUNCTION_H
