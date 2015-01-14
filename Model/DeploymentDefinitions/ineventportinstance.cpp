#include "ineventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"
#include <QDebug>

InEventPortInstance::InEventPortInstance(QString name):Node(Node::NT_INSTANCE)
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
bool InEventPortInstance::canConnect(Node* attachableObject)
{
    InEventPort* inEventPort = dynamic_cast<InEventPort*> (attachableObject);

    if(!inEventPort){
        qWarning() << "InEventPortInstance Node can only be connected to 1 InEventPort Definition.";
        return false;
    }

    if(inEventPort && getDefinition()){
        qWarning() << "InEventPortInstance Node can only be connected to 1 InEventPort Definition.";
        return false;
    }

    return Node::canConnect(attachableObject);
}
