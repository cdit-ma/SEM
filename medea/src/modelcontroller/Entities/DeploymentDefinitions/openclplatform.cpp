#include "openclplatform.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::OPENCL_PLATFORM;
const QString kind_string = "OpenCL Platform";

void OpenCLPlatform::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OpenCLPlatform(broker, is_temp_node);
    });
}

OpenCLPlatform::OpenCLPlatform(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsNodeKind(NODE_KIND::OPENCL_DEVICE);
    //setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Vendor, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Version, QVariant::String, ProtectedState::PROTECTED);
}