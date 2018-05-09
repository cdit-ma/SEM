#include "dds_historyqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_HISTORY;
const static QString kind_string = "DDS_HistoryQosPolicy";

void DDS_HistoryQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_HistoryQosPolicy(factory, is_temp_node);
    });
}

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "KEEP_LAST_HISTORY_QOS";
    values << "KEEP_ALL_HISTORY_QOS";
    factory.AttachData(this, "label", QVariant::String, "history", true);
    factory.AttachData(this, "qos_dds_depth", QVariant::String, "1", false);
    auto dds_kind_data = factory.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}