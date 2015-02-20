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
        qWarning() << "AggregateInstance can only connect to an Aggregate.";
        return false;
    }
    if(getDefinition() && aggregate){
        qWarning() << "AggregateInstance can only connect to one Aggregate.";
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
            qCritical() << "AggregateInstance is already connected in directly to Node";
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
        qWarning() << "AggregateInstance can only adopt MemberInstance or AggregateInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}
