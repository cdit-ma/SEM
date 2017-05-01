#include "vector.h"
#include <QDebug>
#include "../data.h"
#include "../entityfactory.h"

Vector::Vector(EntityFactory* factory) : DataNode(factory, NODE_KIND::VECTOR, "Vector"){
	auto node_kind = NODE_KIND::VECTOR;
	QString kind_string = "Vector";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Vector();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

Vector::Vector(): DataNode(NODE_KIND::VECTOR)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReciever(true);

    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    connect(this, &Node::childCountChanged, this, &Vector::childrenChanged);
}

QString Vector::getVectorType()
{
    QString vectorType = "Vector";
    QString childType;
    Node* child = getFirstChild();
    if(child){
        //Check Type
        switch(child->getNodeKind()){
        case NODE_KIND::MEMBER:
        case NODE_KIND::AGGREGATE_INSTANCE:{
            childType = child->getDataValue("type").toString();
            break;
        }
        default:
            break;
        }
    }
    if(childType != ""){
        childType = "::" + childType;
    }
    return vectorType + childType;
}

bool Vector::canAdoptChild(Node *child)
{
    //Can Only adopt 1x Member/AggregateInstance
    switch(child->getNodeKind()){
    case NODE_KIND::MEMBER:
    case NODE_KIND::AGGREGATE_INSTANCE:
        if(hasChildren()){
            return false;
        }
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Vector::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_AGGREGATE:{
        if(dst->getNodeKind() != NODE_KIND::AGGREGATE){
            return false;
        }
        if(hasChildren()){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}

void Vector::updateType()
{
    //Get Data
    Data* d = getData("type");
    if(d){
        d->setValue(getVectorType());
    }
}

void Vector::childrenChanged()
{
    Node* child = getFirstChild();
    if(child){
        connect(child, &Node::dataChanged, this, &Vector::updateType);
    }
    updateType();
}
