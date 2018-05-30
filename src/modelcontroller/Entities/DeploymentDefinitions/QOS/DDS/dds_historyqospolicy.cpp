#include "dds_historyqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_HISTORY;
const static QString kind_string = "DDS_HistoryQosPolicy";

void DDS_HistoryQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_HistoryQosPolicy(broker, is_temp_node);
    });
}

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "history");
    broker.AttachData(this, "qos_dds_depth", QVariant::String, ProtectedState::UNPROTECTED, "1");
    auto dds_kind_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, ProtectedState::UNPROTECTED);
    dds_kind_data->addValidValues({"KEEP_LAST_HISTORY_QOS", "KEEP_ALL_HISTORY_QOS"});
}