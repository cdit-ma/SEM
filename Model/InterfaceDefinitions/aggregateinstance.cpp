#include "aggregateinstance.h"
#include "memberinstance.h"
#include "aggregate.h"
#include <QDebug>

AggregateInstance::AggregateInstance():Node(Node::NT_DEFINSTANCE)
{
}

AggregateInstance::~AggregateInstance()
{
}

bool AggregateInstance::canConnect(Node* attachableObject)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(attachableObject);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if (!aggregate && !aggregateInstance){
#ifdef DEBUG_MODE
        qWarning() << "AggregateInstance can only connect to an Aggregate.";
#endif
        return false;
    }
    if(getDefinition() && aggregate){
#ifdef DEBUG_MODE
        qWarning() << "AggregateInstance can only connect to one Aggregate.";
#endif
        return false;
    }

    Node* topMostParent= getParentNode();
    while(topMostParent){
        QString parentKind = topMostParent->getDataValue("kind");
        if(parentKind.contains("Aggregate")){
            topMostParent = topMostParent->getParentNode();
        }else{
            break;
        }
    }
    if(!(topMostParent->isImpl() || topMostParent->isInstance()) && !topMostParent->isDefinition()){
        //Check for ownership in the same file, for circular checks
        if(!this->getParentNode()->isImpl()){
            if(aggregate){
                if(!aggregate->getParentNode()->isAncestorOf(this)){
                    return false;
                }
            }
            if(aggregateInstance){
                if(aggregateInstance->getParentNode()->isAncestorOf(this)){
                    return false;
                }
                Node* aDefinition = aggregateInstance->getDefinition();
                if(aDefinition && aDefinition->getParentNode() && !aDefinition->getParentNode()->isAncestorOf(this)){
                    return false;
                }
            }

        }
        //Check for connection.
        if(isIndirectlyConnected(attachableObject)){
#ifdef DEBUG_MODE
            qWarning() << "AggregateInstance is already connected in directly to Node";
#endif
            return false;
        }

    }





    return Node::canConnect(attachableObject);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!memberInstance && !aggregateInstance){
#ifdef DEBUG_MODE
        qWarning() << "AggregateInstance can only adopt MemberInstance or AggregateInstance";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
