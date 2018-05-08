#include "hardwaredefinitions.h"
#include "../../edgekinds.h"
#include "../../entityfactory.h"
#include <QSysInfo>

const NODE_KIND node_kind = NODE_KIND::HARDWARE_DEFINITIONS;
const QString kind_string = "HardwareDefinitions";

HardwareDefinitions::HardwareDefinitions(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareDefinitions();});
    //RegisterComplexNodeKind(factory, node_kind, &HardwareDefinitions::ConstructHardwareDefinitions);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "HARDWARE");
};

HardwareDefinitions::HardwareDefinitions():Node(node_kind)
{
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_CLUSTER);
    setAcceptsNodeKind(NODE_KIND::LOGGINGPROFILE);
    setAcceptsNodeKind(NODE_KIND::LOGGINGSERVER);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);
}

VIEW_ASPECT HardwareDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::HARDWARE;
}

#include <QDebug>

Node* HardwareDefinitions::ConstructHardwareDefinitions(EntityFactory* factory){
    //Don't recurse into the complex function
    auto hardware = factory->CreateNode(NODE_KIND::HARDWARE_DEFINITIONS, false);
    auto localhost = factory->CreateNode(NODE_KIND::HARDWARE_NODE);

    if(hardware && localhost){
        auto adopt_success = hardware->addChild(localhost);

        if(adopt_success){
            auto key_label = factory->GetKey("label", QVariant::String);
            auto key_ip_address = factory->GetKey("ip_address", QVariant::String);
            auto key_os = factory->GetKey("os", QVariant::String);
            auto key_os_version = factory->GetKey("os_version", QVariant::String);
            auto key_architecture = factory->GetKey("architecture", QVariant::String);
            auto key_uuid = factory->GetKey("uuid", QVariant::String);

            factory->AttachData(localhost, key_label, "localhost", true);
            factory->AttachData(localhost, key_ip_address, "127.0.0.1", true);
            factory->AttachData(localhost, key_os, QSysInfo::productType(), true);
            factory->AttachData(localhost, key_os_version, QSysInfo::productVersion(), true);
            factory->AttachData(localhost, key_architecture, QSysInfo::currentCpuArchitecture(), true);
            factory->AttachData(localhost, key_uuid, "localhost", true);

            return hardware;
        }
    }
    
    factory->DestructEntity(hardware);
    factory->DestructEntity(localhost);
    return 0;
}