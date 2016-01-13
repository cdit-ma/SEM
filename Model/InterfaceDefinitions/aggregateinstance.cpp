#include "aggregateinstance.h"
#include "memberinstance.h"
#include "aggregate.h"
#include "vectorinstance.h"
#include <QDebug>

AggregateInstance::AggregateInstance():DataNode(Node::NT_DEFINSTANCE)
{
    setAcceptEdgeClass(Edge::EC_DEFINITION);
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

    return Node::canConnect_DefinitionEdge(definition);
}

bool AggregateInstance::canConnect_DataEdge(Node *node)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(node);

    if(aggregateInstance){
        Node* nDef = node;
        while(nDef->getDefinition()){
            Node* def = nDef->getDefinition();
            if(def){
                nDef = def;
            }else{
                break;
            }
        }

        Node* tDef = this;
        while(tDef->getDefinition()){
            Node* def = tDef->getDefinition();
            if(def){
                tDef = def;
            }else{
                break;
            }
        }
        if(nDef != tDef){
            return false;

        }
        if(aggregateInstance->getInputData()){
            return false;
        }
    }


    return DataNode::canConnect_DataEdge(node);
}
