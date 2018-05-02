#include "dds_presentationqospolicy.h"

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_PRESENTATION, "DDS_PresentationQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_PRESENTATION;
	QString kind_string = "DDS_PresentationQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_PresentationQosPolicy();});

    QList<QVariant> values;
    values << "INSTANCE_PRESENTATION_QOS";
    values << "TOPIC_PRESENTATION_QOS";
    values << "GROUP_PRESENTATION_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_access_scope", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "presentation");
    RegisterDefaultData(factory, node_kind, "qos_dds_access_scope", QVariant::String, false, values.first());
    RegisterDefaultData(factory, node_kind, "qos_dds_coherent_access", QVariant::Bool, false, false);
    RegisterDefaultData(factory, node_kind, "qos_dds_ordered_access", QVariant::Bool, false, false);
}

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_PRESENTATION)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}
