#include "workload.h"
#include <QDebug>
Workload::Workload():Node()
{
    //qCritical() << "Constructed Workload: "<< this->getName();

}

Workload::~Workload()
{
    //Destructor
}

QString Workload::toString()
{
    return QString("Workload[%1]: "+this->getName()).arg(this->getID());
}

bool Workload::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Workload::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
