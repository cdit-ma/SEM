#include "ifcondition.h"

const NODE_KIND node_kind = NODE_KIND::IF_CONDITION;
const QString kind_string = "IfCondition";


MEDEA::IfCondition::IfCondition(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new IfCondition();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "if");
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "Boolean");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};

MEDEA::IfCondition::IfCondition() : Node(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
}


bool MEDEA::IfCondition::canAdoptChild(Node *child)
{
    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool MEDEA::IfCondition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
