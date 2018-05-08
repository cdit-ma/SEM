#include "hardwarecluster.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::HARDWARE_CLUSTER;
const QString kind_string = "HardwareCluster";

void HardwareCluster::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new HardwareCluster(factory, is_temp_node);
        });
}

HardwareCluster::HardwareCluster(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);

    //Setup Data
    factory.AttachData(this, "uuid", QVariant::String, "", true);
    factory.AttachData(this, "url", QVariant::String, "", true);
}