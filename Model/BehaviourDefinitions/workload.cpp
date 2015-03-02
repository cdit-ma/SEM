#include "workload.h"
#include <QDebug>
#include "process.h"
Workload::Workload():Node()
{
    //qWarning() << "Constructed Workload: "<< this->getName();

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
#ifdef DEBUG_MODE
         qWarning() << "Workload can only adopt Process!";
#endif


        return false;
    }
    return Node::canAdoptChild(child);
}
