#include "elseifcondition.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ELSEIF_CONDITION;
const QString kind_string = "ElseIfCondition";

void MEDEA::ElseIfCondition::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ElseIfCondition(broker, is_temp_node);
        });
}

MEDEA::ElseIfCondition::ElseIfCondition(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    setLabelFunctional(false);
    setDataReceiver(true);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "else if", true);
    broker.AttachData(this, "type", QVariant::String, "Boolean", true);
    broker.AttachData(this, "value", QVariant::String, "", false);
}