#include "aggregateinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../edgekinds.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::AGGREGATE_INST;
const QString kind_string = "Aggregate Instance";


void AggregateInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new AggregateInst(broker, is_temp_node);
    });
}

AggregateInst::AggregateInst(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::AGGREGATE);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INST);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INST);

    

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "column_count", QVariant::Int, ProtectedState::UNPROTECTED, 1);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::PROTECTED);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
}

bool AggregateInst::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DATA:{        //Can only connect to an AggregateInstance.
        if(dst->getNodeKind() == NODE_KIND::AGGREGATE_INST){
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


void AggregateInst::ParentSet(DataNode* child){
    if(child->getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        bool got_valid_producer = false;
        bool got_valid_receiver = false;

        QSet<NODE_KIND> valid_producer_parents = {NODE_KIND::VARIABLE};
        QSet<NODE_KIND> valid_receiver_parents = {NODE_KIND::PORT_PUBLISHER_IMPL};
        
        QSet<NODE_KIND> inverse_parents = {NODE_KIND::FUNCTION_CALL, NODE_KIND::PORT_REQUESTER_IMPL};
        QSet<NODE_KIND> invalid_parents = {NODE_KIND::VECTOR, NODE_KIND::VECTOR_INST};

        QSet<NODE_KIND> ancestor_kinds;

        //Make a list of ancestor kinds
        for(auto ancestor : child->getParentNodes(-1)){
            ancestor_kinds += ancestor->getNodeKind();
        }

        

        if(ancestor_kinds.intersects(inverse_parents)){
            valid_producer_parents += {NODE_KIND::RETURN_PARAMETER_GROUP, NODE_KIND::RETURN_PARAMETER_GROUP_INST};
            valid_receiver_parents += {NODE_KIND::INPUT_PARAMETER_GROUP, NODE_KIND::INPUT_PARAMETER_GROUP_INST};
        }else{
            valid_producer_parents += {NODE_KIND::INPUT_PARAMETER_GROUP, NODE_KIND::INPUT_PARAMETER_GROUP_INST};
            valid_receiver_parents += {NODE_KIND::RETURN_PARAMETER_GROUP, NODE_KIND::RETURN_PARAMETER_GROUP_INST};
        }

        if(!ancestor_kinds.intersects(valid_receiver_parents)){
            valid_producer_parents += {NODE_KIND::PORT_SUBSCRIBER_IMPL};
        }

        //No Data Linking in the Interface Definitions aspect except for Enum Members
        if(ancestor_kinds.contains(NODE_KIND::INTERFACE_DEFINITIONS) && child->getNodeKind() != NODE_KIND::ENUM_MEMBER){
            //Fail out
            invalid_parents = ancestor_kinds;
        }

        if(!ancestor_kinds.intersects(invalid_parents)){
            if(ancestor_kinds.intersects(valid_producer_parents)){
                got_valid_producer = true;
            }
            if(ancestor_kinds.intersects(valid_receiver_parents)){
                got_valid_receiver = true;
            }
        }
        
        if(child->getParentNodeKind() == NODE_KIND::VARIABLE){
            got_valid_producer = false;
            got_valid_receiver = false;
        }

        if(ancestor_kinds.contains(NODE_KIND::FUNCTION_CALL)){
            if(ancestor_kinds.contains(NODE_KIND::INPUT_PARAMETER_GROUP_INST) && ancestor_kinds.contains(NODE_KIND::AGGREGATE_INST)){
                got_valid_receiver = false;
            }
        }

        child->setDataProducer(got_valid_producer);
        child->setDataReceiver(got_valid_receiver);
    }
}
void AggregateInst::parentSet(Node* parent){
    switch(parent->getNodeKind()){
        case NODE_KIND::PORT_SUBSCRIBER_IMPL:{
            getFactoryBroker().AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
            getFactoryBroker().AttachData(this, "column", QVariant::Int, ProtectedState::PROTECTED, -1);
            break;
        }
        case NODE_KIND::AGGREGATE:
        case NODE_KIND::VECTOR:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            getFactoryBroker().AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
            SetEdgeRuleActive(EdgeRule::DISALLOW_DEFINITION_CHAINING);
            break;
        }
        default:
            break;
    }

    AggregateInst::ParentSet(this);
    DataNode::parentSet(parent);
}