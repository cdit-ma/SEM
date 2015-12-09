#include "ineventportdelegate.h"

InEventPortDelegate::InEventPortDelegate():EventPortDelegate(true)
{
}

InEventPortDelegate::~InEventPortDelegate()
{
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortDelegate::canConnect_AssemblyEdge(Node *node)
{
    return EventPortDelegate::canConnect_AssemblyEdge(node);
}

