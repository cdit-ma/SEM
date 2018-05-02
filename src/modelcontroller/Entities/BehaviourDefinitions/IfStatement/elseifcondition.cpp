#include "elseifcondition.h"

const NODE_KIND node_kind = NODE_KIND::ELSEIF_CONDITION;
const QString kind_string = "ElseIfCondition";


MEDEA::ElseIfCondition::ElseIfCondition(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ElseIfCondition();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "else if");
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "Boolean");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};

MEDEA::ElseIfCondition::ElseIfCondition() : DataNode(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setDataReceiver(true);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
}