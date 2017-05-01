#include "parameter.h"
#include "../nodekinds.h"

#include "../entityfactory.h"

Parameter::Parameter(EntityFactory* factory, NODE_KIND kind, QString kind_str) : DataNode(factory, kind, kind_str){
    
    RegisterDefaultData(factory, kind, "type", QVariant::String, true);
    
    if(kind != NODE_KIND::VARIADIC_PARAMETER){
        RegisterDefaultData(factory, kind, "icon", QVariant::String, true);
        RegisterDefaultData(factory, kind, "icon_prefix", QVariant::String, true);
        RegisterDefaultData(factory, kind, "description", QVariant::String, true);
    }
};

Parameter::Parameter(NODE_KIND kind):DataNode(kind)
{
    setNodeType(NODE_TYPE::PARAMETER);
    setAcceptsEdgeKind(Edge::EC_DATA);

    setMoveEnabled(false);
    setExpandEnabled(false);
}

bool Parameter::isInputParameter() const
{
    return getNodeKind() == NODE_KIND::INPUT_PARAMETER;
}

bool Parameter::isVariadicParameter() const
{
    return getNodeKind() == NODE_KIND::VARIADIC_PARAMETER;
}

bool Parameter::isReturnParameter() const
{
    return getNodeKind() == NODE_KIND::RETURN_PARAMETER;
}

bool Parameter::isVariableParameter() const
{
    return getNodeKind() == NODE_KIND::VARIABLE_PARAMETER;
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
        if(dst->isNodeOfType(NODE_TYPE::PARAMETER)){
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
