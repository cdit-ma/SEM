#include "componentassembly.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_ASSEMBLY;
const QString kind_string = "Component Assembly";

void ComponentAssembly::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ComponentAssembly(broker, is_temp_node);
        });
}

ComponentAssembly::ComponentAssembly(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::PORT_PUBLISHER_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "replicate_count", QVariant::Int, ProtectedState::UNPROTECTED, 1);
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
}