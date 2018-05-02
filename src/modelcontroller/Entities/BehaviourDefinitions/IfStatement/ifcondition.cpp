#include "ifcondition.h"

const NODE_KIND node_kind = NODE_KIND::IF_CONDITION;
const QString kind_string = "IfCondition";


MEDEA::IfCondition::IfCondition(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new IfCondition();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "if");
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "Boolean");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};

MEDEA::IfCondition::IfCondition() : DataNode(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setDataReceiver(true);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
}