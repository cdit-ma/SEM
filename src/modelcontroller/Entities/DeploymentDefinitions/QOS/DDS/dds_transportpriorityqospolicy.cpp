#include "dds_transportpriorityqospolicy.h"

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY, "DDS_TransportPriorityQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY;
	QString kind_string = "DDS_TransportPriorityQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_TransportPriorityQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "transport_priority");
    RegisterDefaultData(factory, node_kind, "qos_dds_int_value", QVariant::Int, false, 0);
}

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}