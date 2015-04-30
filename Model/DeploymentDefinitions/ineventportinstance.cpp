#include "ineventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include <QDebug>

InEventPortInstance::InEventPortInstance(QString name):Node(Node::NT_INSTANCE)
{
}

InEventPortInstance::~InEventPortInstance()
{

}


bool InEventPortInstance::canAdoptChild(Node *child)
{
    return false;
    /*AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);


    if(!aggregateInstance || (aggregateInstance && this->childrenCount() > 0)){
#ifdef DEBUG_MODE
        qWarning() << "InEventPortInstance can only adopt one AggregateInstance";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
*/
}


//An InEventPortInstance can be connected to:
//Connected to a Definition:
// + InEventPort
bool InEventPortInstance::canConnect(Node* attachableObject)
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
