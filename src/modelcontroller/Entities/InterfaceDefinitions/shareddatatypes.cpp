#include "shareddatatypes.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::SHARED_DATATYPES;
const QString kind_string = "Shared Datatypes";

void SharedDatatypes::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new SharedDatatypes(broker, is_temp_node);
    });
}

SharedDatatypes::SharedDatatypes(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::AGGREGATE);
    setAcceptsNodeKind(NODE_KIND::ENUM);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);
    setAcceptsNodeKind(NODE_KIND::SERVER_INTERFACE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::UUID, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Version, QVariant::String, ProtectedState::UNPROTECTED, "v1.0");
    broker.AttachData(this, KeyName::ColumnCount, QVariant::Int, ProtectedState::UNPROTECTED, 5);
}