#include "hardwarecluster.h"
//#include "../entityfactory.h"

HardwareCluster::HardwareCluster(EntityFactory* factory) : Node(factory, NODE_KIND::HARDWARE_CLUSTER, "HardwareCluster"){
	auto node_kind = NODE_KIND::HARDWARE_CLUSTER;
	QString kind_string = "HardwareCluster";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareCluster();});

    //TODO register data
};

HardwareCluster::HardwareCluster():Node(NODE_KIND::HARDWARE_CLUSTER)
{
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);
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

bool HardwareCluster::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
