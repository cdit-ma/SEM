#include "memberinstance.h"
#include "../InterfaceDefinitions/member.h"

MemberInstance::MemberInstance():Node(Node::NT_DEFINSTANCE)
{
    addValidEdgeType(Edge::EC_DEFINITION);
    addValidEdgeType(Edge::EC_DATA);
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

bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}
