#include "blanknode.h"
#include <qdebug>
BlankNode::BlankNode(QString name):Node()
{
    //qDebug() << "Constructed Blank Node: "<< this->getName();
}

BlankNode::~BlankNode()
{

}

bool BlankNode::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool BlankNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
