#include "hardwarecluster.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::HARDWARE_CLUSTER;
const QString kind_string = "Hardware Cluster";

void HardwareCluster::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new HardwareCluster(broker, is_temp_node);
        });
}

HardwareCluster::HardwareCluster(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, KeyName::UUID, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Url, QVariant::String, ProtectedState::PROTECTED);
}