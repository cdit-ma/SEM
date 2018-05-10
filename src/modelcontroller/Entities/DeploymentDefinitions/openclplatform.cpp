#include "openclplatform.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::OPENCL_PLATFORM;
const static QString kind_string = "OpenCLPlatform";

void OpenCLPlatform::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OpenCLPlatform(broker, is_temp_node);
    });
}

OpenCLPlatform::OpenCLPlatform(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsNodeKind(NODE_KIND::OPENCL_DEVICE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);



    
    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "", true);
    broker.AttachData(this, "vendor", QVariant::String, "", true);
    broker.AttachData(this, "version", QVariant::String, "", true);
}