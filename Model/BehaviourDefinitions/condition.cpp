#include "condition.h"
#include <QDebug>
Condition::Condition():Node()
{
    //qCritical() << "Constructed Condition: "<< this->getName();
}

Condition::~Condition()
{
    //Destructor
}

QString Condition::toString()
{
    return QString("Condition[%1]: "+this->getName()).arg(this->getID());
}

bool Condition::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Condition::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
