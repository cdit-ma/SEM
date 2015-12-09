#include "componentinstance.h"
#include "attributeinstance.h"
#include "eventportinstance.h"
#include "../InterfaceDefinitions/component.h"

ComponentInstance::ComponentInstance():Node(Node::NT_INSTANCE)
{
    addValidEdgeType(Edge::EC_DEPLOYMENT);
    addValidEdgeType(Edge::EC_DEFINITION);
}

ComponentInstance::~ComponentInstance()
{
}

bool ComponentInstance::canAdoptChild(Node *child)
{
    AttributeInstance* attributeInstance = dynamic_cast<AttributeInstance*>(child);
    EventPortInstance* eventPortInstance = dynamic_cast<EventPortInstance*>(child);

    if(!(attributeInstance || eventPortInstance)){
        //ComponentInstance can only adopt AttributeInstance/EventPortInstance
        return false;
    }

    return Node::canAdoptChild(child);
}

bool ComponentInstance::canConnect_DefinitionEdge(Node *definition)
{
    Component* component = dynamic_cast<Component*>(definition);
    if(!component){
        return false;
    }
    return Node::canConnect_DefinitionEdge(definition);
}
