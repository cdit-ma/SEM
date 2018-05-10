#include "aggregateinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../edgekinds.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::AGGREGATE_INSTANCE;
const QString kind_string = "AggregateInstance";


void AggregateInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new AggregateInstance(broker, is_temp_node);
    });
}

AggregateInstance::AggregateInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::AGGREGATE);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);




    //Setup Data
    broker.AttachData(this, "index", QVariant::Int, -1);
    broker.AttachData(this, "type", QVariant::String, "", true);
}

bool AggregateInstance::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DATA:{        //Can only connect to an AggregateInstance.
        if(dst->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE){
            //Can only connect to an AggregateInstance with the same definition.
            if(!getDefinition(true) || getDefinition(true) != dst->getDefinition(true)){
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


void AggregateInstance::ParentSet(DataNode* child){
    if(child->getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        bool got_valid_producer = false;
        bool got_valid_receiver = false;

        QSet<NODE_KIND> valid_producer_parents = {NODE_KIND::INEVENTPORT_IMPL, NODE_KIND::VARIABLE};
        QSet<NODE_KIND> valid_receiver_parents = {NODE_KIND::OUTEVENTPORT_IMPL};
        QSet<NODE_KIND> invalid_parents = {NODE_KIND::VECTOR, NODE_KIND::VECTOR_INSTANCE};

        QSet<NODE_KIND> special_parents = {NODE_KIND::RETURN_PARAMETER_GROUP, NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE, NODE_KIND::INPUT_PARAMETER_GROUP, NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE};


        //Check our ancestor type
        for(auto ancestor : child->getParentNodes(-1)){
            auto ancestor_kind = ancestor->getNodeKind();
            auto parent_node_kind = ancestor->getParentNodeKind();

            if(!got_valid_receiver && !got_valid_producer && valid_producer_parents.contains(ancestor_kind)){
                got_valid_producer = true;
            }

            if(!got_valid_receiver && valid_receiver_parents.contains(ancestor_kind)){
                got_valid_receiver = true;
            }

            if(special_parents.contains(ancestor_kind)){
                QSet<NODE_KIND> inverse_direction = {NODE_KIND::SERVER_REQUEST, NODE_KIND::FUNCTION_CALL};

                auto invert_direction = inverse_direction.contains(parent_node_kind);

                switch(ancestor_kind){
                    case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
                    case NODE_KIND::INPUT_PARAMETER_GROUP:{
                        got_valid_producer = invert_direction ? false : true;
                        got_valid_receiver = invert_direction ? true : false;
                        break;
                    }
                    case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:
                    case NODE_KIND::RETURN_PARAMETER_GROUP:{
                        got_valid_producer = invert_direction ? true : false;
                        got_valid_receiver = invert_direction ? false : true;
                        break;
                    }
                    default:
                        break;
                }
                break;
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