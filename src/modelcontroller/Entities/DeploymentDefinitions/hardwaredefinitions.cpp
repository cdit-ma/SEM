#include "hardwaredefinitions.h"
#include "../../edgekinds.h"
#include "../../entityfactory.h"
#include <QSysInfo>

const NODE_KIND node_kind = NODE_KIND::HARDWARE_DEFINITIONS;
const QString kind_string = "HardwareDefinitions";

void HardwareDefinitions::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new HardwareDefinitions(factory, is_temp_node);
    });
}

HardwareDefinitions::HardwareDefinitions(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_CLUSTER);
    setAcceptsNodeKind(NODE_KIND::LOGGINGPROFILE);
    setAcceptsNodeKind(NODE_KIND::LOGGINGSERVER);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "HARDWARE", true);

    //Attach Children
    auto localhost = factory.ConstructChildNode(*this, NODE_KIND::HARDWARE_NODE);

    factory.AttachData(localhost, "label", QVariant::String, "localhost", true);
    factory.AttachData(localhost, "ip_address", QVariant::String, "127.0.0.1", true);
    factory.AttachData(localhost, "os", QVariant::String, QSysInfo::productType(), true);
    factory.AttachData(localhost, "os_version", QVariant::String, QSysInfo::productVersion(), true);
    factory.AttachData(localhost, "architecture", QVariant::String, QSysInfo::currentCpuArchitecture(), true);
    factory.AttachData(localhost, "uuid", QVariant::String, "localhost", true);
}


VIEW_ASPECT HardwareDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::HARDWARE;
}
