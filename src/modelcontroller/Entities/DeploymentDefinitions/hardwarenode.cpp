#include "hardwarenode.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const auto node_kind = NODE_KIND::HARDWARE_NODE;
const QString kind_string = "Hardware Node";

void HardwareNode::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new HardwareNode(broker, is_temp_node);
    });
}

HardwareNode::HardwareNode(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::OPENCL_PLATFORM);
    setLabelFunctional();

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "ip_address", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "os", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "os_version", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "architecture", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "uuid", QVariant::String, ProtectedState::PROTECTED);
}