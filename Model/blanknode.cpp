#include "blanknode.h"
#include <qdebug>
BlankNode::BlankNode(QString name):Node()
{
    //qDebug() << "Constructed Blank Node: "<< this->getName();
}

BlankNode::~BlankNode()
{

}

QString BlankNode::toString()
{
    return QString("BlankNode[%1]: "+this->getName()).arg(this->getID());
}

bool BlankNode::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool BlankNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
