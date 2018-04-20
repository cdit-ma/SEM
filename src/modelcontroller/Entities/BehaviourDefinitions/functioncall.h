#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "../Node.h"

class EntityFactory;
class FunctionCall: public Node
{
public:
	FunctionCall(EntityFactory* factory);
	FunctionCall();

    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
	bool canAdoptChild(Node* child);
	QList<Node*> getAdoptableNodes(Node* definition);
};

#endif // FUNCTIONCALL_H
