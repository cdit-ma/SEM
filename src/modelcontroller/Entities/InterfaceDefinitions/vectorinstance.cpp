#include "vectorinstance.h"


VectorInstance::VectorInstance(EntityFactory* factory) : DataNode(factory, NODE_KIND::VECTOR_INSTANCE, "VectorInstance"){
	auto node_kind = NODE_KIND::VECTOR_INSTANCE;
	QString kind_string = "VectorInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new VectorInstance();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

VectorInstance::VectorInstance(): DataNode(NODE_KIND::VECTOR_INSTANCE)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReceiver(true);

    setDefinitionKind(NODE_KIND::VECTOR);
    setInstanceKind(NODE_KIND::VECTOR_INSTANCE);
    setImplKind(NODE_KIND::VECTOR_INSTANCE);

    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);
}

bool VectorInstance::canAdoptChild(Node *child)
{
    //Can only adopt a MemberInstance/AggregateInstance
    switch(child->getNodeKind()){
    case NODE_KIND::MEMBER_INSTANCE:
    case NODE_KIND::ENUM_INSTANCE:
    case NODE_KIND::AGGREGATE_INSTANCE:{
        if(hasChildren()){
            return false;
        }
        break;
    }
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool VectorInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
        if(!(dst->getNodeKind() == NODE_KIND::VECTOR_INSTANCE || dst->getNodeKind() == NODE_KIND::VECTOR)){
            return false;
        }
        break;
    }
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
    return DataNode::canAcceptEdge(edgeKind, dst);
}
