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
    //Check for existing connection.
    if(isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return true;

}

bool BlankNode::canAdoptChild(Node *child)
{
    if(this->isAncestorOf(child) || this->isDescendantOf(child)){
        return false;
    }

    return true;
}
