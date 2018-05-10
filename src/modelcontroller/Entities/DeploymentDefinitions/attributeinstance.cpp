#include "attributeinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE_INSTANCE;
const QString kind_string = "AttributeInstance";


void AttributeInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new AttributeInstance(broker, is_temp_node);
        });
}

AttributeInstance::AttributeInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::ATTRIBUTE);
    setChainableDefinition();



    
    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "ASSEMBLIES", true);
    broker.AttachData(this, "value", QVariant::String, "", false);
}

void AttributeInstance::parentSet(Node* parent){
    if(getViewAspect() == VIEW_ASPECT::ASSEMBLIES){
        setDataReceiver(true);
    }else if(getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        setDataProducer(true);
    }
    DataNode::parentSet(parent);
}