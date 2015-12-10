#include "attributeimpl.h"
#include "../InterfaceDefinitions/attribute.h"
#include <QDebug>

/**
 * @brief AttributeImpl::AttributeImpl Constructor
 * Calls Node::NT_IMPL as it is an Implementation of Attribute
 */
AttributeImpl::AttributeImpl():BehaviourNode(Node::NT_IMPL)
{
    setIsNonWorkflow(true);
    setIsDataInput(true);
    setIsDataOutput(true);
    setAcceptEdgeClass(Edge::EC_DATA);
    setAcceptEdgeClass(Edge::EC_DEFINITION);
}

AttributeImpl::~AttributeImpl()
{
}


bool AttributeImpl::canAdoptChild(Node*)
{
    return false;
}

bool AttributeImpl::canConnect_DataEdge(Node *node)
{
    return BehaviourNode::canConnect_DataEdge(node);
}

bool AttributeImpl::canConnect_DefinitionEdge(Node *definition)
{
    Attribute* attribute = dynamic_cast<Attribute*>(definition);

    if(!attribute){
        return false;
    }

    //Call base impl to check rules.
    return Node::canConnect_DefinitionEdge(definition);
}


