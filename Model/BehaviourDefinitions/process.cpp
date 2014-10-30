#include "process.h"

Process::Process():Node()
{
    //qCritical() << "Constructed PeriodicEvent: "<< this->getName();

}

Process::~Process()
{
    //Destructor
}

QString Process::toString()
{
    return QString("Process[%1]: "+this->getName()).arg(this->getID());
}

bool Process::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Process::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
