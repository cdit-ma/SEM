#include <QDebug>
#include "vectorinstance.h"
#include "aggregateinstance.h"
#include "memberinstance.h"
#include "vector.h"
#include "../BehaviourDefinitions/inputparameter.h"

VectorInstance::VectorInstance(): Node(Node::NT_DEFINSTANCE)
{
    addValidEdgeType(Edge::EC_DEFINITION);
    addValidEdgeType(Edge::EC_DATA);
}

VectorInstance::~VectorInstance()
{
}

bool VectorInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!(aggregateInstance || memberInstance)){
        return false;
    }
    if(hasChildren()){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool VectorInstance::canConnect_DefinitionEdge(Node *definition)
{
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(definition);
    Vector* vector = dynamic_cast<Vector*>(definition);

    if(!(vector || vectorInstance)){
        return false;
    }

    if(getDefinition()){
        return false;
    }

    return Node::canConnect_DefinitionEdge(definition);
}

bool VectorInstance::canConnect_DataEdge(Node *node)
{
    return Node::canConnect_DataEdge(node);
}
