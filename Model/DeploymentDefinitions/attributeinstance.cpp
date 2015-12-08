#include "attributeinstance.h"
#include "../InterfaceDefinitions/attribute.h"

AttributeInstance::AttributeInstance():Node(Node::NT_INSTANCE)
{
    addValidEdgeType(Edge::EC_DEFINITION);
}

AttributeInstance::~AttributeInstance()
{
}


bool AttributeInstance::canAdoptChild(Node*)
{
    return false;
}

bool AttributeInstance::canConnect_DefinitionEdge(Node *definition)
{
    Attribute* attribute = dynamic_cast<Attribute*>(definition);
    if(!attribute){
        return false;
    }

    return Node::canConnect_DefinitionEdge(definition);
}
