#include "termination.h"
#include <QDebug>
Termination::Termination():Node()
{
    //qCritical() << "Constructed Termination: "<< this->getName();
}

Termination::~Termination()
{
    //Destructor
}

bool Termination::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Termination::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
