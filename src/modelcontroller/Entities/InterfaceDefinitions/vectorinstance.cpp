#include "vectorinstance.h"


VectorInstance::VectorInstance(EntityFactory* factory) : DataNode(factory, NODE_KIND::VECTOR_INSTANCE, "VectorInstance"){
	auto node_kind = NODE_KIND::VECTOR_INSTANCE;
	QString kind_string = "VectorInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new VectorInstance();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
};

VectorInstance::VectorInstance(): DataNode(NODE_KIND::VECTOR_INSTANCE)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReceiver(true);


    addInstancesDefinitionKind(NODE_KIND::VECTOR);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
}

bool VectorInstance::canAdoptChild(Node *child)
{
   
    if(childrenCount() > 0){
        return false;
    }
    return Node::canAdoptChild(child);
}

bool VectorInstance::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    switch(edge_kind){
    case EDGE_KIND::DATA:{
        if(dst->getNodeKind() == NODE_KIND::VECTOR_INSTANCE){
            if(getDefinition(true) && dst->getDefinition(true) != getDefinition(true)){
                return false;
            }
        }

        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edge_kind, dst);
}
