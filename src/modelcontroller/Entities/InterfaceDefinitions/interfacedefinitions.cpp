#include "interfacedefinitions.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INTERFACE_DEFINITIONS;
const QString kind_string = "Interface Definitions";

void InterfaceDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new InterfaceDefinitions(broker, is_temp_node);
        });
}

InterfaceDefinitions::InterfaceDefinitions(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::ENUM);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE);
    setAcceptsNodeKind(NODE_KIND::COMPONENT);
    setAcceptsNodeKind(NODE_KIND::SHARED_DATATYPES);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);
    setAcceptsNodeKind(NODE_KIND::SERVER_INTERFACE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    setLabelFunctional(false);
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "INTERFACES");
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::INTERFACES;
}