#include "ineventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include <QDebug>

InEventPortInstance::InEventPortInstance():Node(Node::NT_INSTANCE)
{
}

InEventPortInstance::~InEventPortInstance()
{

}


bool InEventPortInstance::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}


//An InEventPortInstance can be connected to:
//Connected to a Definition:
// + InEventPort
Edge::EDGE_CLASS InEventPortInstance::canConnect(Node* attachableObject)
{
    InEventPort* inEventPort = dynamic_cast<InEventPort*> (attachableObject);

    if(inEventPort && getDefinition()){
#ifdef DEBUG_MODE
        qWarning() << "InEventPortInstance Node can only be connected to 1 InEventPort Definition.";
#endif
        return false;
    }

    if(!inEventPort){
#ifdef DEBUG_MODE
        qWarning() << "InEventPortInstance Node can only be connected to InEventPorts.";
#endif
        return false;
    }

    return Node::canConnect(attachableObject);
}
