#include "variable.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "../InterfaceDefinitions/vectorinstance.h"

Variable::Variable():BehaviourNode()
{
    addValidEdgeType(Edge::EC_DATA);
}

Variable::~Variable()
{
}

bool Variable::canAdoptChild(Node* child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);

    if(!(aggregateInstance || vectorInstance)){
        //Variable can only adopt a VectorInstance or an AggregateInstance
        return false;
    }

    if(hasChildren()){
        //Variable can only adopt 1 child.
        return false;
    }

    return BehaviourNode::canAdoptChild(child);
}
