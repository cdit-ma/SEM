#include "forcondition.h"
//#include "../entityfactory.h"

ForCondition::ForCondition(EntityFactory* factory) : Condition(factory, NODE_KIND::FOR_CONDITION, "ForCondition"){
	auto node_kind = NODE_KIND::FOR_CONDITION;
	QString kind_string = "ForCondition";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ForCondition();});
};

ForCondition::ForCondition():Condition(NODE_KIND::FOR_CONDITION){
    
}

bool ForCondition::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::VARIABLE_PARAMETER:
        case NODE_KIND::INPUT_PARAMETER:
            break;
    default:
        return false;
    }
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);
}

bool ForCondition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Condition::canAcceptEdge(edgeKind, dst);
}



