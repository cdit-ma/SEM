#include "hardwaredefinitions.h"
#include "../../edgekinds.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include <QSysInfo>

const NODE_KIND node_kind = NODE_KIND::HARDWARE_DEFINITIONS;
const QString kind_string = "Hardware Definitions";

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
    setLabelFunctional(false);
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "HARDWARE");

    //Attach Children
    auto localhost = broker.ConstructChildNode(*this, NODE_KIND::HARDWARE_NODE);

    broker.AttachData(localhost, "label", QVariant::String, ProtectedState::PROTECTED, "localhost");
    broker.AttachData(localhost, "ip_address", QVariant::String, ProtectedState::PROTECTED, "127.0.0.1");
    broker.AttachData(localhost, "os", QVariant::String, ProtectedState::PROTECTED, QSysInfo::productType());
    broker.AttachData(localhost, "os_version", QVariant::String, ProtectedState::PROTECTED, QSysInfo::productVersion());
    broker.AttachData(localhost, "architecture", QVariant::String, ProtectedState::PROTECTED,  QSysInfo::currentCpuArchitecture());
    broker.AttachData(localhost, "uuid", QVariant::String,ProtectedState::PROTECTED,  "localhost");
}


VIEW_ASPECT HardwareDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::HARDWARE;
}
