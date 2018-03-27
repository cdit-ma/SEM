#ifndef WORKERFUNCTION_H
#define WORKERFUNCTION_H

#include "BehaviourDefinitions/process.h"

class EntityFactory;
class WorkerFunction: public Process
{
public:
	WorkerFunction(EntityFactory* factory);
	WorkerFunction();

    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKERFUNCTION_H
