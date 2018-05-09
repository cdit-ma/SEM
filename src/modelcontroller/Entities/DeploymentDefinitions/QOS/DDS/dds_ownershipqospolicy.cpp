#include "dds_ownershipqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIP;
const static QString kind_string = "DDS_OwnershipQosPolicy";

void DDS_OwnershipQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_OwnershipQosPolicy(factory, is_temp_node);
    });
}

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "SHARED_OWNERSHIP_QOS";
    values << "EXCLUSIVE_OWNERSHIP_QOS";
    factory.AttachData(this, "label", QVariant::String, "ownership", true);
    auto dds_kind_data = factory.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}