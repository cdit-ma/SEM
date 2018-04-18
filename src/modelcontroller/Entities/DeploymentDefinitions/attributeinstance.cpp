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
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setNodeType(NODE_TYPE::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    
    setDefinitionKind(NODE_KIND::ATTRIBUTE);
    setInstanceKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    

    setDataProducer(true);
    
}

bool AttributeInstance::canAdoptChild(Node*)
{
    return false;
}

bool AttributeInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::ATTRIBUTE:
            case NODE_KIND::ATTRIBUTE_INSTANCE:
                break;
            default:
                return false;
        }
        break;
    }
    
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}

void AttributeInstance::parentSet(Node* parent){
    if(getViewAspect() == VIEW_ASPECT::ASSEMBLIES){
        setDataReceiver(true);
    }
    DataNode::parentSet(parent);
}