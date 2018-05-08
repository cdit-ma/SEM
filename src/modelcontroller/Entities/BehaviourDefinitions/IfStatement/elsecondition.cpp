#include "elsecondition.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ELSE_CONDITION;
const QString kind_string = "ElseCondition";

void MEDEA::ElseCondition::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ElseCondition(factory, is_temp_node);
        });
}

MEDEA::ElseCondition::ElseCondition(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //SetupState
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "else", true);
}