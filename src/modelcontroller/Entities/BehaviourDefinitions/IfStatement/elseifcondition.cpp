#include "elseifcondition.h"

const NODE_KIND node_kind = NODE_KIND::ELSEIF_CONDITION;
const QString kind_string = "ElseIfCondition";


MEDEA::ElseIfCondition::ElseIfCondition(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ElseIfCondition();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "else if");
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "Boolean");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};

MEDEA::ElseIfCondition::ElseIfCondition() : Node(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
}


bool MEDEA::ElseIfCondition::canAdoptChild(Node *child)
{
    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool MEDEA::ElseIfCondition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
