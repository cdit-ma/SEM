#include "dds_presentationqospolicy.h"

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_PRESENTATION)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "presentation");
    updateDefaultData("qos_dds_access_scope", QVariant::String, false, "INSTANCE_PRESENTATION_QOS");
    updateDefaultData("qos_dds_coherent_access", QVariant::Bool, false, false);
    updateDefaultData("qos_dds_ordered_access", QVariant::Bool, false, false);

    //qos_dds_access_scope
    auto values = QStringList();
    values << "INSTANCE_PRESENTATION_QOS";
    values << "TOPIC_PRESENTATION_QOS";
    values << "GROUP_PRESENTATION_QOS";
}

bool DDS_PresentationQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PresentationQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}