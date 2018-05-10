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
    if (is_temp) {
        return;
    }

    // Setup State
    addInstanceKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE, EDGE_DIRECTION::TARGET);
    
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::VECTOR);




    // Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "namespace", QVariant::String, "", true);
    broker.AttachData(this, "comment", QVariant::String, "");

}

void Aggregate::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace" || key_name == "type"){
        TypeKey::BindNamespaceAndLabelToType(this, true);
    }
}