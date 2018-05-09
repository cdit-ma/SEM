#include "dds_transportpriorityqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY;
const static QString kind_string = "DDS_TransportPriorityQosPolicy";

void DDS_TransportPriorityQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_TransportPriorityQosPolicy(factory, is_temp_node);
    });
}

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "transport_priority", true);
    factory.AttachData(this, "qos_dds_int_value", QVariant::Int, 0, false);
}