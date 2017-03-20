#include "vector.h"
#include <QDebug>
#include "../data.h"
Vector::Vector(): Node(Node::NK_VECTOR)
{
    setNodeType(NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    connect(this, &Node::childCountChanged, this, &Vector::childrenChanged);
}

QString Vector::getVectorType()
{
    QString vectorType = "vector";
    QString childType;
    Node* child = getFirstChild();
    if(child){
        //Check Type
        switch(child->getNodeKind()){
        case NK_MEMBER:{
            childType = child->getDataValue("type").toString();
            break;
        }
        case NK_AGGREGATE_INSTANCE:{
            vectorType = "complex_vector";
            childType = child->getDataValue("type").toString();
            break;
        }
        default:
            break;
        }
    }
    if(childType != ""){
        childType = "<" + childType + ">";
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
    return Node::canAcceptEdge(edgeKind, dst);
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
