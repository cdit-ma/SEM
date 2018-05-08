#include "elseifcondition.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ELSEIF_CONDITION;
const QString kind_string = "ElseIfCondition";

void MEDEA::ElseIfCondition::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ElseIfCondition(factory, is_temp_node);
        });
}

MEDEA::ElseIfCondition::ElseIfCondition(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //SetupState
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    setLabelFunctional(false);
    setDataReceiver(true);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "else if", true);
    factory.AttachData(this, "type", QVariant::String, "Boolean", true);
    factory.AttachData(this, "value", QVariant::String, "", false);
}