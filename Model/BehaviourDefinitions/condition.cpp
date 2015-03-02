#include "condition.h"
#include <QDebug>
Condition::Condition():Node()
{
    //qWarning() << "Constructed Condition: "<< this->getName();
}

Condition::~Condition()
{
    //Destructor
}

bool Condition::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Condition::canAdoptChild(Node *child)
{
    return false;
}
