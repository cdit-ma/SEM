#include "outeventportinstance.h"

#include "../node.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "ineventportinstance.h"
#include "outeventportdelegate.h"
#include "ineventportdelegate.h"
#include <QDebug>

OutEventPortInstance::OutEventPortInstance(QString name):Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed OutEventPortIDL: "<< this->getName();
}

OutEventPortInstance::~OutEventPortInstance()
{

}


bool OutEventPortInstance::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!aggregateInstance || (aggregateInstance && this->childrenCount() > 0)){
        qWarning() << "OutEventPortInstance can only adopt one AggregateInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}

bool OutEventPortInstance::canConnect(Node* attachableObject)
{
    OutEventPort* outEventPort = dynamic_cast<OutEventPort*> (attachableObject);

    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(attachableObject);
    OutEventPortDelegate* outEventPortDelegate = dynamic_cast<OutEventPortDelegate*>(attachableObject);
    InEventPortDelegate* inEventPortDelegate = dynamic_cast<InEventPortDelegate*>(attachableObject);

    if(!outEventPortDelegate && !inEventPortInstance && !outEventPort && !inEventPortDelegate){
        qCritical() << "OutEventPortInstance can only be connected to InEventPortInstances (Which match the same definition), 1 OutEventPort and defined InEventPortDelegates.";
        return false;
    }

    if(outEventPort && getDefinition()){
        qWarning() << "OutEventPortInstance can only be connected to 1 OutEventPort Definition.";
        return false;
    }

    if(outEventPortDelegate || inEventPortInstance){
           EventPort* oEP = dynamic_cast<EventPort*>(getDefinition());
           if(!oEP || !oEP->getAggregate()){
               qWarning() << "OutEventPortInstance cannot be connected until it has a connected Aggregate in it's definition.";
               return false;
           }
    }

    if(outEventPortDelegate || inEventPortDelegate || inEventPortInstance){
        EventPort* outDefinition = dynamic_cast<EventPort*>(getDefinition());
        EventPort* outDefinition2  = 0;

        if(outEventPortDelegate){
            OutEventPortInstance* oEPI = outEventPortDelegate->getOutEventPortInstance();
            if(oEPI){
                outDefinition2 = dynamic_cast<EventPort*>(oEPI->getDefinition());
            }
        }

        if(inEventPortDelegate){
            InEventPortInstance* iEPI = inEventPortDelegate->getInEventPortInstance();
            if(iEPI){
                outDefinition2 = dynamic_cast<EventPort*>(iEPI->getDefinition());
            }
        }
        if(inEventPortInstance){
            outDefinition2 =  dynamic_cast<EventPort*>(inEventPortInstance->getDefinition());
        }


        if(outDefinition2){
            if(outDefinition->getAggregate() != outDefinition2->getAggregate()){
                qWarning() << "OutEventPortInstance cannot be connected to a Delegate with a different Aggregate definition.";
                return false;
            }
        }
    }

    if(inEventPortInstance){
        if(inEventPortInstance->getParentNode() == this->getParentNode()){
            qWarning() << "OutEventPortInstance cannot be connected to an InEventPortInstance Contained by the same ComponentInstance Node!";
            return false;
        }
        if(inEventPortInstance->getParentNode()->getParentNode() != this->getParentNode()->getParentNode()){
            qWarning() << "OutEventPortInstance must only be connected to an InEventPortInstance contained in this assembly!";
            return false;
        }
    }

    if(outEventPortDelegate){
        if(!outEventPortDelegate->getParentNode()->isAncestorOf(this)){
            qWarning() << "OutEventPortInstance cannot be connected to an OutEventPortDelegate not contained in the same Assembly!";
            return false;
        }
        if(outEventPortDelegate->getParentNode() != this->getParentNode()->getParentNode()){
            qWarning() << "OutEventPortInstance must only be connected to an OutEventPortDelegate contained in this assembly!";
            return false;
        }
    }

    if(inEventPortDelegate){
        if(!this->getParentNode()->getParentNode()->isAncestorOf(inEventPortDelegate)){
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate not contained in the same Assembly!";
            return false;
        }
        if(inEventPortDelegate->getParentNode()->getParentNode() != this->getParentNode()->getParentNode()){
            qWarning() << "OutEventPortInstance must only be connected to an OutEventPortDelegate contained in this assembly!";
            return false;
        }

    }

    return Node::canConnect(attachableObject);
}
