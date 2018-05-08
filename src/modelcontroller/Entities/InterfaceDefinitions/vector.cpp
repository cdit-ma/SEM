#include "vector.h"
#include "../Keys/typekey.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::VECTOR;
const QString kind_string = "SharedDatatypes";

void Vector::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Vector(factory, is_temp_node);
    });
}

Vector::Vector(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReceiver(true);

    addInstanceKind(NODE_KIND::VECTOR_INSTANCE);   
    
    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);

    //Setup Data
    factory.AttachData(this, "icon", QVariant::String, "", true);
    factory.AttachData(this, "icon_prefix", QVariant::String, "", true);
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
    TypeKey::BindInnerAndOuterTypes(child, this, true);
}

void Vector::childRemoved(Node* child){
    Vector::updateVectorIcon(this);
    DataNode::childRemoved(child);
    TypeKey::BindInnerAndOuterTypes(child, this, false);
}
