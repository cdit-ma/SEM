#include "dds_qosprofile.h"

DDS_QOSProfile::DDS_QOSProfile():Node(NK_QOS_DDS_PROFILE)
{
    setNodeType(NT_QOS_PROFILE);
    setAcceptsEdgeKind(Edge::EC_QOS);
}

bool DDS_QOSProfile::canAdoptChild(Node *node)
{
    if(!node->isNodeOfType(NT_QOS_DDS_POLICY)){
        return false;
    }

    if(!getChildrenOfKind(node->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(node);
}

bool DDS_QOSProfile::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
