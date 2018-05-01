#include "attributeinstance.h"

#include "../../edgekinds.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE_INSTANCE;
const QString kind_string = "AttributeInstance";


AttributeInstance::AttributeInstance(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new AttributeInstance();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

AttributeInstance::AttributeInstance() : DataNode(node_kind)
{
    addInstancesDefinitionKind(NODE_KIND::ATTRIBUTE);
    setChainableDefinition();
}

bool AttributeInstance::canAdoptChild(Node*)
{
    return false;
}

bool AttributeInstance::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}

void AttributeInstance::parentSet(Node* parent){
    if(getViewAspect() == VIEW_ASPECT::ASSEMBLIES){
        setDataReceiver(true);
    }else if(getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        setDataProducer(true);
    }
    DataNode::parentSet(parent);
}