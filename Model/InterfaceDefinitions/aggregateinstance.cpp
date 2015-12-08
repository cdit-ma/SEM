#include "aggregateinstance.h"
#include "memberinstance.h"
#include "aggregate.h"
#include "vectorinstance.h"
#include <QDebug>

AggregateInstance::AggregateInstance():Node(Node::NT_DEFINSTANCE)
{
}

AggregateInstance::~AggregateInstance()
{
}

Edge::EDGE_CLASS AggregateInstance::canConnect(Node* attachableObject)
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
    }

    return Node::canConnect(attachableObject);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);

    if(!memberInstance && !aggregateInstance  && !vectorInstance){
#ifdef DEBUG_MODE
        qWarning() << "AggregateInstance can only adopt MemberInstance or AggregateInstance or VectorInstance";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
