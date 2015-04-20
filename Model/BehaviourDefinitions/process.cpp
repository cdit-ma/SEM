#include "process.h"

Process::Process():Node()
{
    //qWarning() << "Constructed PeriodicEvent: "<< this->getName();

}

Process::~Process()
{
    //Destructor
}

bool Process::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
    //return Node::canConnect(attachableObject);
}

bool Process::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false; //Node::canAdoptChild(child);
}
