#include "workload.h"
#include <QDebug>
#include "process.h"
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
    Process* process = dynamic_cast<Process*>(child);
    if(!process){
        qCritical() << "Workload can only adopt Process!";
        return false;
    }
    return Node::canAdoptChild(child);
}
