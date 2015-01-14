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

bool Workload::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Workload::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
