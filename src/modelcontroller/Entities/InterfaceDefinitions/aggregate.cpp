#include "aggregate.h"
#include "../data.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../edgekinds.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::AGGREGATE;
const QString kind_string = "Aggregate";


void Aggregate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Aggregate(broker, is_temp_node);
    });
}


Aggregate::Aggregate(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    // Setup State
    addInstanceKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE, EDGE_DIRECTION::TARGET);
    
    setAcceptsNodeKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::VECTOR);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    // Setup Data
    broker.AttachData(this, "column_count", QVariant::Int, ProtectedState::UNPROTECTED, 1);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "namespace", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
    TypeKey::BindNamespaceAndLabelToType(this, true);
}

