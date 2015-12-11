#include "memberinstance.h"
#include "../InterfaceDefinitions/member.h"

MemberInstance::MemberInstance():BehaviourNode(Node::NT_DEFINSTANCE)
{
    setIsNonWorkflow(true);
    setIsDataInput(true);
    setIsDataOutput(true);
    setAcceptEdgeClass(Edge::EC_DEFINITION);
    setAcceptEdgeClass(Edge::EC_DATA);
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
    int depthToAspectChild = getDepthToAspect() - 1;

    Node* aspectChild = getParentNode(depthToAspectChild);
    if(aspectChild){
        if(!aspectChild->isImpl()){
            //Cannot have data edges for anything outside of an impl.
            return false;
        }
    }

    return BehaviourNode::canConnect_DataEdge(node);
}

bool MemberInstance::canConnect_WorkflowEdge(Node *node)
{
    return false;
}

bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}
