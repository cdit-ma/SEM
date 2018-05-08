#include "hardwarenode.h"
#include "../../entityfactory.h"

const auto node_kind = NODE_KIND::HARDWARE_NODE;
const QString kind_string = "HardwareNode";

void HardwareNode::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new HardwareNode(factory, is_temp_node);
    });
}

HardwareNode::HardwareNode(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::OPENCL_PLATFORM);
    setLabelFunctional();

    factory.AttachData(this, "label", QVariant::String, "", true);
    factory.AttachData(this, "ip_address", QVariant::String, "", true);
    factory.AttachData(this, "os", QVariant::String, "", true);
    factory.AttachData(this, "os_version", QVariant::String, "", true);
    factory.AttachData(this, "architecture", QVariant::String, "", true);
    factory.AttachData(this, "uuid", QVariant::String, "", true);
}