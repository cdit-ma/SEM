#include "hardwarecluster.h"

#include "../../edgekinds.h"

HardwareCluster::HardwareCluster(EntityFactory* factory) : Node(factory, NODE_KIND::HARDWARE_CLUSTER, "HardwareCluster"){
	auto node_kind = NODE_KIND::HARDWARE_CLUSTER;
	QString kind_string = "HardwareCluster";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareCluster();});
    RegisterDefaultData(factory, node_kind, "uuid", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "url", QVariant::String, true);
};

HardwareCluster::HardwareCluster():Node(NODE_KIND::HARDWARE_CLUSTER)
{
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);

}