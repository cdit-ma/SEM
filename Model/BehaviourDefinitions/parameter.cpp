#include "parameter.h"
#include "behaviournode.h"
#include "../InterfaceDefinitions/memberinstance.h"
#include "variable.h"
#include "attributeimpl.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include <QDebug>

Parameter::Parameter(bool isInput):BehaviourNode()
{
    setIsNonWorkflow(true);
    setAcceptEdgeClass(Edge::EC_DATA);
    inputParameter = isInput;
}

bool Parameter::isInputParameter()
{
    return inputParameter;
}

bool Parameter::isReturnParameter()
{
    return !inputParameter;
}

bool Parameter::hasConnection()
{
    return hasEdges();
}


bool Parameter::canAdoptChild(Node*)
{
    return false;
}

bool Parameter::canConnect_DataEdge(Node *node)
{
    Parameter* parameter = dynamic_cast<Parameter*>(node);
    if(parameter){
        if(parameter->getParentNode() == getParentNode()){
            //cannot connect 2 Parameters from the same parent.
            return false;
        }
    }
    return BehaviourNode::canConnect_DataEdge(node);

}
