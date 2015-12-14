#include "memberinstance.h"
#include "../InterfaceDefinitions/member.h"

MemberInstance::MemberInstance():DataNode(Node::NT_DEFINSTANCE)
{
    setAcceptEdgeClass(Edge::EC_DEFINITION);
}

MemberInstance::~MemberInstance()
{
}

bool MemberInstance::canConnect_DefinitionEdge(Node *definition)
{
    Member* member = dynamic_cast<Member*>(definition);
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(definition);

    if(!(member || memberInstance)){
        return false;
    }

    return Node::canConnect_DefinitionEdge(definition);
}

bool MemberInstance::canConnect_DataEdge(Node *node)
{
    DataNode::canConnect_DataEdge(node);
}

bool MemberInstance::canConnect_WorkflowEdge(Node *node)
{
    return false;
}

bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}
