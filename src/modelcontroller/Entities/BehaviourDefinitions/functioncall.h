#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "../node.h"

class EntityFactory;
class FunctionCall: public Node
{
public:
	FunctionCall(EntityFactory* factory);
	FunctionCall();

    //bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
	bool canAdoptChild(Node* child);
protected:
	QSet<Node*> getListOfValidAncestorsForChildrenDefinitions() override;
	QSet<Node*> getParentNodesForValidDefinition() override;
private:
    Node* getTopBehaviourContainer();
    bool top_behaviour_calculated = false;
    Node* top_behaviour_container = 0;
};

#endif // FUNCTIONCALL_H
