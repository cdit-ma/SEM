#include "ifcondition.h"
#include "../containernode.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::IF_CONDITION;
const QString kind_string = "IfCondition";

void MEDEA::IfCondition::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::IfCondition(factory, is_temp_node);
        });
}

MEDEA::IfCondition::IfCondition(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    setLabelFunctional(false);
    setDataReceiver(true);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "if", true);
    factory.AttachData(this, "type", QVariant::String, "Boolean", true);
    factory.AttachData(this, "value", QVariant::String, "", false);
}