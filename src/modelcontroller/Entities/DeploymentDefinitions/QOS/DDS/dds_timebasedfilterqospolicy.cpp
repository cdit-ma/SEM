#include "dds_timebasedfilterqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER;
const static QString kind_string = "DDS_TimeBasedFilterQosPolicy";

void DDS_TimeBasedFilterQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_TimeBasedFilterQosPolicy(factory, is_temp_node);
    });
}

DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "time_based_filter", true);
    factory.AttachData(this, "qos_dds_minimum_separation_sec", QVariant::String, "0", false);
    factory.AttachData(this, "qos_dds_minimum_separation_nanosec", QVariant::String, "0", false);
}