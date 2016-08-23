#include "hardwarecluster.h"

HardwareCluster::HardwareCluster():Node(NK_HARDWARE_CLUSTER)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

bool HardwareCluster::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_HARDWARE_NODE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool HardwareCluster::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
