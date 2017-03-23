#include "parameter.h"
#include "../InterfaceDefinitions/datanode.h"
#include <QDebug>
Parameter::Parameter(Node::NODE_KIND kind):DataNode(kind)
{
    setNodeType(NT_PARAMETER);
    setAcceptsEdgeKind(Edge::EC_DATA);
}

bool Parameter::isInputParameter() const
{
    return getNodeKind() == NK_INPUT_PARAMETER;
}

bool Parameter::isVariadicParameter() const
{
    return getNodeKind() == NK_VARIADIC_PARAMETER;
}

bool Parameter::isReturnParameter() const
{
    return getNodeKind() == NK_RETURN_PARAMETER;
}

bool Parameter::isVariableParameter() const
{
    return getNodeKind() == NK_VARIABLE_PARAMETER;
}

bool Parameter::canAdoptChild(Node*)
{
    return false;
}

bool Parameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DATA:{
        if(dst->isNodeOfType(NT_PARAMETER)){
            Parameter* parameter = (Parameter*) dst;
            //Allow connection to things in the same component
            if(getDepthFromCommonAncestor(dst) == 1){
                return false;
            }
            /*
            if(!isReturnParameter()){
                //Only allow connections from Return Parameters
                return false;
            }
            if(!(parameter->isInputParameter() || parameter->isVariadicParameter() || parameter)){
                //Allow connection to Input Parameters or VariadicParameter
                return false;
            }*/
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);

}
