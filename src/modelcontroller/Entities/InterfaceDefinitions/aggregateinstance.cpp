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

    //Only allow data binding for particular cases based on ancestor
    removeEdgeKind(EDGE_KIND::DATA);
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


void AggregateInstance::ParentSet(DataNode* child){
    if(child->getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        bool got_valid_producer = false;
        bool got_valid_receiver = false;

        QSet<NODE_KIND> valid_producer_parents = {NODE_KIND::INEVENTPORT_IMPL, NODE_KIND::VARIABLE};
        QSet<NODE_KIND> valid_receiver_parents = {NODE_KIND::OUTEVENTPORT_IMPL, NODE_KIND::VARIABLE};
        QSet<NODE_KIND> invalid_parents = {NODE_KIND::VECTOR, NODE_KIND::VECTOR_INSTANCE};

        QSet<NODE_KIND> special_parents = {NODE_KIND::RETURN_PARAMETER_GROUP, NODE_KIND::INPUT_PARAMETER_GROUP};


        //Check our ancestor type
        for(auto ancestor : child->getParentNodes(-1)){
            auto ancestor_kind = ancestor->getNodeKind();

            if(!got_valid_producer && valid_producer_parents.contains(ancestor_kind)){
                got_valid_producer = true;
            }
            if(!got_valid_receiver && valid_receiver_parents.contains(ancestor_kind)){
                got_valid_receiver = true;
            }

            if(special_parents.contains(ancestor_kind)){
                auto parent = ancestor->getParentNode();
                QSet<NODE_KIND> inverse_direction = {NODE_KIND::SERVER_REQUEST};

                auto invert_direction = inverse_direction.contains(parent->getNodeKind());

                switch(ancestor_kind){
                    case NODE_KIND::INPUT_PARAMETER_GROUP:{
                        got_valid_producer = invert_direction ? false : true;
                        got_valid_receiver = invert_direction ? true : false;
                        break;
                    }
                    case NODE_KIND::RETURN_PARAMETER_GROUP:{
                        got_valid_producer = invert_direction ? true : false;
                        got_valid_receiver = invert_direction ? false : true;
                        break;
                    }
                    default:
                        break;
                }
            }
            
            if(invalid_parents.contains(ancestor_kind)){
                got_valid_producer = false;
                got_valid_receiver = false;
                break;
            }
        }
        child->setDataProducer(got_valid_producer);
        child->setDataReceiver(got_valid_receiver);
    }
}
void AggregateInstance::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}