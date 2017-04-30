#include "dds_qosprofile.h"

DDS_QOSProfile::DDS_QOSProfile():Node(NODE_KIND::QOS_DDS_PROFILE)
{
    setNodeType(NODE_TYPE::QOS_PROFILE);
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);
    setAcceptsEdgeKind(Edge::EC_QOS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, false, "qos_profile");
}

bool DDS_QOSProfile::canAdoptChild(Node *node)
{
    if(!node->isNodeOfType(NODE_TYPE::QOS)){
        return false;
    }
    if(!node->isNodeOfType(NODE_TYPE::DDS)){
        return false;
    }
    if(node->isNodeOfType(NODE_TYPE::QOS_PROFILE)){
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