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

QString Termination::toString()
{
    return QString("Termination[%1]: "+this->getName()).arg(this->getID());
}

bool Termination::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Termination::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
