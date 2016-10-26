#include "hardwarecluster.h"

HardwareCluster::HardwareCluster():Node(NK_HARDWARE_CLUSTER)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setNodeType(NT_HARDWARE);
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

bool HardwareCluster::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
