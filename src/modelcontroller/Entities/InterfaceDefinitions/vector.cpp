#include "vector.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "aggregateinstance.h"

const NODE_KIND node_kind = NODE_KIND::VECTOR;
const QString kind_string = "Vector";

void Vector::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Vector(broker, is_temp_node);
    });
}

Vector::Vector(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReceiver(true);

    addInstanceKind(NODE_KIND::VECTOR_INSTANCE);   
    
    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "outer_type", QVariant::String, ProtectedState::PROTECTED, "Vector");
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
}

bool Vector::canAdoptChild(Node *child)
{
    if(getChildrenCount() > 0){
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Vector::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::AGGREGATE:{
        if(dst->getNodeKind() != NODE_KIND::AGGREGATE){
            return false;
        }
        if(getChildrenCount() > 0){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edge_kind, dst);
}

void Vector::updateVectorIcon(Node* node){
    auto children = node->getChildren(0);
    auto child_kind = children.size() ? children.first()->getNodeKind() : NODE_KIND::NONE;
    
    auto icon = node->getDataValue("kind").toString();

    switch(child_kind){
        case NODE_KIND::NONE:
            break;
        case NODE_KIND::AGGREGATE_INSTANCE:
            icon += "_AggregateInstance";
            break;
        default:
            icon += "_Member";
            break;
    }
    node->setDataValue("icon_prefix", "EntityIcons");
    node->setDataValue("icon", icon);
}

void Vector::childAdded(Node* child){
    Vector::updateVectorIcon(this);

    DataNode::childAdded(child);
    Vector::BindVectorTypes(this, child, true);
}

void Vector::childRemoved(Node* child){
    Vector::updateVectorIcon(this);
    DataNode::childRemoved(child);
    Vector::BindVectorTypes(this, child, false);
}

void Vector::BindVectorTypes(Node* vector, Node* child, bool bind){
    auto src_inner_type_data = vector->getData("inner_type");
    auto child_type_data = child->getData("type");

    //Got fully described data
    if(src_inner_type_data && child_type_data){
        child_type_data->linkData(src_inner_type_data, bind);
    }
}

void Vector::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}