#include "hardwarecluster.h"

HardwareCluster::HardwareCluster():Node(NODE_KIND::HARDWARE_CLUSTER)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setNodeType(NODE_TYPE::HARDWARE);

    
}

bool HardwareCluster::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::HARDWARE_NODE:
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
