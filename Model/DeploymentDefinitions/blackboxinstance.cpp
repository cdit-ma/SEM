#include "blackboxinstance.h"
#include "../InterfaceDefinitions/blackbox.h"
#include "eventportinstance.h"

BlackBoxInstance::BlackBoxInstance():Node(Node::NT_INSTANCE)
{
    setAcceptEdgeClass(Edge::EC_DEPLOYMENT);
    setAcceptEdgeClass(Edge::EC_DEFINITION);
}

BlackBoxInstance::~BlackBoxInstance()
{
}

bool BlackBoxInstance::canAdoptChild(Node *child)
{
    EventPortInstance* eventPortInstance = dynamic_cast<EventPortInstance*>(child);

    if(!eventPortInstance){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool BlackBoxInstance::canConnect_DefinitionEdge(Node *definition)
{
    BlackBox* blackBox = dynamic_cast<BlackBox*>(definition);

    if(!blackBox){
        return false;
    }
    return Node::canConnect_DefinitionEdge(definition);
}
