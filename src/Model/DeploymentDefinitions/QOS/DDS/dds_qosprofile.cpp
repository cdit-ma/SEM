#include "dds_qosprofile.h"

DDS_QOSProfile::DDS_QOSProfile():Node(NK_QOS_DDS_PROFILE)
{
    setNodeType(NT_QOS_PROFILE);
    setNodeType(NT_QOS);
    setNodeType(NT_DDS);
    setAcceptsEdgeKind(Edge::EC_QOS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, false, "qos_profile");
}

bool DDS_QOSProfile::canAdoptChild(Node *node)
{
    if(!node->isNodeOfType(NT_QOS)){
        return false;
    }
    if(!node->isNodeOfType(NT_DDS)){
        return false;
    }
    if(node->isNodeOfType(NT_QOS_PROFILE)){
        return false;
    }

    if(!getChildrenOfKind(node->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(node);
}

bool DDS_QOSProfile::canAcceptEdge(Edge::EDGE_KIND , Node *)
{
    return false;
}