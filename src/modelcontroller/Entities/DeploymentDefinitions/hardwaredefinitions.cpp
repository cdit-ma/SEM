#include "hardwaredefinitions.h"
#include "../../edgekinds.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include <QSysInfo>

const NODE_KIND node_kind = NODE_KIND::HARDWARE_DEFINITIONS;
const QString kind_string = "HardwareDefinitions";

void HardwareDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new HardwareDefinitions(broker, is_temp_node);
    });
}

HardwareDefinitions::HardwareDefinitions(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_CLUSTER);
    setAcceptsNodeKind(NODE_KIND::LOGGINGPROFILE);
    setAcceptsNodeKind(NODE_KIND::LOGGINGSERVER);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "HARDWARE", true);

    //Attach Children
    auto localhost = broker.ConstructChildNode(*this, NODE_KIND::HARDWARE_NODE);

    broker.AttachData(localhost, "label", QVariant::String, "localhost", true);
    broker.AttachData(localhost, "ip_address", QVariant::String, "127.0.0.1", true);
    broker.AttachData(localhost, "os", QVariant::String, QSysInfo::productType(), true);
    broker.AttachData(localhost, "os_version", QVariant::String, QSysInfo::productVersion(), true);
    broker.AttachData(localhost, "architecture", QVariant::String, QSysInfo::currentCpuArchitecture(), true);
    broker.AttachData(localhost, "uuid", QVariant::String, "localhost", true);
}


VIEW_ASPECT HardwareDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::HARDWARE;
}
