#include "attributeinstance.h"

//#include "../entityfactory.h"
AttributeInstance::AttributeInstance(EntityFactory* factory) : Node(factory, NODE_KIND::ATTRIBUTE_INSTANCE, "AttributeInstance"){
	auto node_kind = NODE_KIND::ATTRIBUTE_INSTANCE;
	QString kind_string = "AttributeInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new AttributeInstance();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

AttributeInstance::AttributeInstance():Node(NODE_KIND::ATTRIBUTE_INSTANCE)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setDefinitionKind(NODE_KIND::ATTRIBUTE);
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
        if(dst->getNodeKind() != NODE_KIND::ATTRIBUTE){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
