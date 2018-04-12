#include "elsecondition.h"

const NODE_KIND node_kind = NODE_KIND::ELSE_CONDITION;
const QString kind_string = "ElseCondition";


MEDEA::ElseCondition::ElseCondition(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ElseCondition();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "else");
};

MEDEA::ElseCondition::ElseCondition() : Node(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
}


bool MEDEA::ElseCondition::canAdoptChild(Node *child)
{
    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool MEDEA::ElseCondition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
