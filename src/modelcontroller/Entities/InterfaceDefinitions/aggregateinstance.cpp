#include "aggregateinstance.h"

#include "../../edgekinds.h"
#include <QDebug>

AggregateInstance::AggregateInstance(EntityFactory* factory) : DataNode(factory, NODE_KIND::AGGREGATE_INSTANCE, "AggregateInstance"){
	auto node_kind = NODE_KIND::AGGREGATE_INSTANCE;
	QString kind_string = "AggregateInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new AggregateInstance();});

    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};
AggregateInstance::AggregateInstance():DataNode(NODE_KIND::AGGREGATE_INSTANCE)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setInstanceKind(NODE_KIND::AGGREGATE_INSTANCE);
    setDefinitionKind(NODE_KIND::AGGREGATE);
    setImplKind(NODE_KIND::AGGREGATE_INSTANCE);
    

    setDataProducer(true);
    setDataReciever(true);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ENUM_INSTANCE:
    case NODE_KIND::AGGREGATE_INSTANCE:
    case NODE_KIND::MEMBER_INSTANCE:
    case NODE_KIND::VECTOR_INSTANCE:
        break;
    default:
        return false;
    }
    return DataNode::canAdoptChild(child);
}

bool AggregateInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case EDGE_KIND::DATA:{        //Can only connect to an AggregateInstance.
        if(dst->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE){
            //Can only connect to an AggregateInstance with the same definition.
            if(!getDefinition(true) || getDefinition(true) != dst->getDefinition(true)){
                return false;
            }
        }

        break;
    }
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE || dst->getNodeKind() == NODE_KIND::AGGREGATE)){
            return false;
        }
        break;
    }
    default:
        break;

    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}

