#include "outeventportdelegate.h"

OutEventPortDelegate::OutEventPortDelegate():EventPortDelegate(false)
{
}

OutEventPortDelegate::~OutEventPortDelegate()
{
}

bool OutEventPortDelegate::canAdoptChild(Node *)
{
    return false;
}

bool OutEventPortDelegate::canConnect_AssemblyEdge(Node *node)
{
    EventPortInstance* eventPortInstance = dynamic_cast<EventPortInstance*>(node);
    if(eventPortInstance){
        if(eventPortInstance->isOutEventPortInstance()){
            return false;
        }
    }
    return EventPortDelegate::canConnect_AssemblyEdge(node);
}
