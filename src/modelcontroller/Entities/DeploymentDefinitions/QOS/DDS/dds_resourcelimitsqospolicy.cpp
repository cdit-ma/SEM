#include "dds_resourcelimitsqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS;
const static QString kind_string = "DDS_ResourceLimitsQosPolicy";

void DDS_ResourceLimitsQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_ResourceLimitsQosPolicy(factory, is_temp_node);
    });
}

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "resource_limits", true);
    factory.AttachData(this, "qos_dds_max_samples", QVariant::String, "LENGTH_UNLIMITED", false);
    factory.AttachData(this, "qos_dds_max_instances", QVariant::String, "LENGTH_UNLIMITED", false);
    factory.AttachData(this, "qos_dds_max_samples_per_instance", QVariant::String, "LENGTH_UNLIMITED", false);
}