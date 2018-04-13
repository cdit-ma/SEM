#ifndef WORKERFUNCTIONCALL_H
#define WORKERFUNCTIONCALL_H

#include "BehaviourDefinitions/process.h"

class EntityFactory;
class WorkerFunctionCall: public Node
{
public:
	WorkerFunctionCall(EntityFactory* factory);
	WorkerFunctionCall();

    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
	bool canAdoptChild(Node* child);
};

#endif // WORKERFUNCTIONCALL_H
