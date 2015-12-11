#include "variable.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include "attributeimpl.h"

Variable::Variable():BehaviourNode()
{
    setIsNonWorkflow(true);
    setIsDataInput(true);
    setIsDataOutput(true);
    setAcceptEdgeClass(Edge::EC_DATA);
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

bool Variable::canConnect_DataEdge(Node *node)
{
    AttributeImpl* attributeImpl = dynamic_cast<AttributeImpl*>(node);
    Variable* variable = dynamic_cast<Variable*>(node);

    if(attributeImpl){
        //Cannot data connect an Attribute to an Attribute.
        return false;
    }
    if(variable){
        //Cannot data connect to a Variable.
        return false;
    }
    return BehaviourNode::canConnect_DataEdge(node);
}
