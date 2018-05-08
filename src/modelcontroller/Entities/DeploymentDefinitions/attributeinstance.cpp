#include "attributeinstance.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE_INSTANCE;
const QString kind_string = "AttributeInstance";


void AttributeInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new AttributeInstance(factory, is_temp_node);
        });
}

AttributeInstance::AttributeInstance(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::ATTRIBUTE);
    setChainableDefinition();

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "ASSEMBLIES", true);
    factory.AttachData(this, "value", QVariant::String, "", false);
}

void AttributeInstance::parentSet(Node* parent){
    if(getViewAspect() == VIEW_ASPECT::ASSEMBLIES){
        setDataReceiver(true);
    }else if(getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        setDataProducer(true);
    }
    DataNode::parentSet(parent);
}