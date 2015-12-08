#include "aggregateinstance.h"
#include "memberinstance.h"
#include "aggregate.h"
#include "vectorinstance.h"
#include <QDebug>

AggregateInstance::AggregateInstance():Node(Node::NT_DEFINSTANCE)
{
    addValidEdgeType(Edge::EC_DEFINITION);
}

AggregateInstance::~AggregateInstance()
{
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);

    if(!(aggregateInstance || memberInstance || vectorInstance)){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool AggregateInstance::canConnect_DefinitionEdge(Node *definition)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(definition);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(definition);

    if(!(aggregate || aggregateInstance)){
        return false;
    }

    //Get the depth to the child of the idl.
    int thisAspectDepth = getDepthToAspect();
    int definitionAspectDepth = definition->getDepthToAspect();

    Node* thisIDL = getParentNode(thisAspectDepth - 1);
    Node* definitionIDL = definition->getParentNode(definitionAspectDepth - 1);

    //If both Aggregate and AggregateInstance are contained in the same parent.
    if(thisIDL == definitionIDL){
        Node* thisIDLChild = getParentNode(thisAspectDepth - 2);
        //Check for cycles in AggregateInstances
        if(thisIDLChild){
            if(definition->isIndirectlyConnected(thisIDLChild)){
                return false;
            }
        }
    }

    return Node::canConnect_DefinitionEdge(definition);
}
