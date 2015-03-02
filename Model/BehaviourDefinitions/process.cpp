#include "process.h"

Process::Process():Node()
{
    //qCritical() << "Constructed PeriodicEvent: "<< this->getName();

}

Process::~Process()
{
    //Destructor
}

bool Process::canConnect(Node* attachableObject)
{

    return Node::canConnect(attachableObject);
}

bool Process::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
