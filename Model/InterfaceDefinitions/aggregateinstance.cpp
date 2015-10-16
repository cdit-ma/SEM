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
    if(aggregateInstance && getDefinition()){
#ifdef DEBUG_MODE
        qWarning() << "AggregateInstance can only connect to an AggregateInstance which has a definition.";
#endif
        return false;
    }

    Node* srcParent = this;
    while(srcParent){
        if(srcParent->getNodeKind() == "AggregateInstance"){
            srcParent = srcParent->getParentNode();
        }else{
            break;
        }
    }

    Node* dstParent = attachableObject;
    while(dstParent){
        if(dstParent->getNodeKind() == "AggregateInstance"){
            dstParent = dstParent->getParentNode();
        }else{
            break;
        }
    }

    //If both Aggregate and AggregateInstance are in the same parent container (IDL)
    if(srcParent->getParentNode() == dstParent->getParentNode()){
        //Check if they are indirectly connected already (Aka check for cycles of AggregateINstances.
        if(attachableObject->isIndirectlyConnected(srcParent)){
            qCritical() << "AggregateInstance is already connected in directly to Node";
            return false;
        }
    }else{
        //

    }
/*


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

    }


    //Check for connection.





*/

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
