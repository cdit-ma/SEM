#include "elsecondition.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ELSE_CONDITION;
const QString kind_string = "ElseCondition";

void MEDEA::ElseCondition::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ElseCondition(broker, is_temp_node);
        });
}

MEDEA::ElseCondition::ElseCondition(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
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
    broker.AttachData(this, "label", QVariant::String, "else", true);
}