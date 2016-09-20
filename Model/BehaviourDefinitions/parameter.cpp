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
    return getNodeKind() == NK_INPUTPARAMETER;
}

bool Parameter::isReturnParameter() const
{
    return getNodeKind() == NK_RETURNPARAMETER;
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
            if(getDepthFromCommonAncestor(dst) == 1){
                return false;
            }
            if(isInputParameter() == parameter->isInputParameter()){
                return false;
            }
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);

}
