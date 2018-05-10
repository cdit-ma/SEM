#include "ifcondition.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::IF_CONDITION;
const QString kind_string = "IfCondition";

void MEDEA::IfCondition::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::IfCondition(broker, is_temp_node);
        });
}

MEDEA::IfCondition::IfCondition(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
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
    broker.AttachData(this, "label", QVariant::String, "if", true);
    broker.AttachData(this, "type", QVariant::String, "Boolean", true);
    broker.AttachData(this, "value", QVariant::String, "", false);
}