#include "dds_destinationorderqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::MODEL;
const static QString kind_string = "Model";

void DDS_DestinationOrderQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_DestinationOrderQosPolicy(factory, is_temp_node);
    });
}

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS";
    values << "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS";
    factory.AttachData(this, "label", QVariant::String, "destination_order", true);
    auto dds_kind_data = factory.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}