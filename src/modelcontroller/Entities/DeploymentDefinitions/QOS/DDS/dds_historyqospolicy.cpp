#include "dds_historyqospolicy.h"

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_HISTORY, "DDS_HistoryQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_HISTORY;
	QString kind_string = "DDS_HistoryQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_HistoryQosPolicy();});

    QList<QVariant> values;
    values << "KEEP_LAST_HISTORY_QOS";
    values << "KEEP_ALL_HISTORY_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "history");
    RegisterDefaultData(factory, node_kind, "qos_dds_kind", QVariant::String, false, values.first());
    RegisterDefaultData(factory, node_kind, "qos_dds_depth", QVariant::String, false, "1");
};

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_HISTORY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}