#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "../node.h"

class EntityFactory;
class FunctionCall: public Node
{
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	FunctionCall(EntityFactory& factory, bool is_temp_node);

	bool canAdoptChild(Node* child);
	QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
	QSet<Node*> getParentNodesForValidDefinition();
private:
    Node* getTopBehaviourContainer();
    bool top_behaviour_calculated = false;
    Node* top_behaviour_container = 0;
};

#endif // FUNCTIONCALL_H
