#include "blanknode.h"
#include <QDebug>
BlankNode::BlankNode():Node()
{
    //qDebug() << "Constructed Blank Node: "<< this->getName();
}

BlankNode::~BlankNode()
{

}

Edge::EDGE_CLASS BlankNode::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool BlankNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
