#include "parameter.h"

#include "../../edgekinds.h"



Parameter::Parameter(EntityFactory* factory, NODE_KIND node_kind, QString kind_str) : DataNode(factory, node_kind, kind_str){
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "inner_type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "outer_type", QVariant::String, true);
};

Parameter::Parameter(NODE_KIND kind):DataNode(kind)
{
    setNodeType(NODE_TYPE::PARAMETER);
    
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

bool Parameter::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}
