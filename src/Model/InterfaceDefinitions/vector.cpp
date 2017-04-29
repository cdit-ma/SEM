#include "vector.h"
#include <QDebug>
#include "../data.h"
Vector::Vector(): DataNode(Node::NK_VECTOR)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReciever(true);

    setNodeType(NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    connect(this, &Node::childCountChanged, this, &Vector::childrenChanged);

    
    updateDefaultData("type", QVariant::String, true);
}

QString Vector::getVectorType()
{
    QString vectorType = "Vector";
    QString childType;
    Node* child = getFirstChild();
    if(child){
        //Check Type
        switch(child->getNodeKind()){
        case NK_MEMBER:
        case NK_AGGREGATE_INSTANCE:{
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
    case NK_MEMBER:
    case NK_AGGREGATE_INSTANCE:
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
        if(dst->getNodeKind() != NK_AGGREGATE){
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
