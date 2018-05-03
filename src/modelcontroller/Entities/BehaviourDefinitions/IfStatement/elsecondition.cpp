#include "elsecondition.h"
#include "../containernode.h"

const NODE_KIND node_kind = NODE_KIND::ELSE_CONDITION;
const QString kind_string = "ElseCondition";


MEDEA::ElseCondition::ElseCondition(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ElseCondition();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "else");
};

MEDEA::ElseCondition::ElseCondition() : DataNode(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
}