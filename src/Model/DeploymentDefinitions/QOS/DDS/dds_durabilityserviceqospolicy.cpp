#include "dds_durabilityserviceqospolicy.h"
#include "../../../data.h"

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy():Node(NK_QOS_DDS_POLICY_DURABILITYSERVICE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DurabilityServiceQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityServiceQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_DurabilityServiceQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();

    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "durability_service");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_service_cleanup_delay_sec
        auto key = Key::GetKey("qos_dds_service_cleanup_delay_sec", QVariant::Int);
        data_list.append(new Data(key, 0));
    }
    {
        //qos_dds_service_cleanup_delay_nanosec
        auto key = Key::GetKey("qos_dds_service_cleanup_delay_nanosec", QVariant::Int);
        data_list.append(new Data(key, 0));
    }
    {
        //qos_dds_history_kind
        auto key = Key::GetKey("qos_dds_history_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "KEEP_LAST_HISTORY_QOS";
            values << "KEEP_ALL_HISTORY_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "KEEP_LAST_HISTORY_QOS"));
    }
    {
        //qos_dds_history_depth
        auto key = Key::GetKey("qos_dds_history_depth", QVariant::Int);
        data_list.append(new Data(key, 1));
    }
    {
        //qos_dds_max_samples
        auto key = Key::GetKey("qos_dds_max_samples", QVariant::String);
        data_list.append(new Data(key, "LENGTH_UNLIMITED"));
    }
    {
        //qos_dds_max_instances
        auto key = Key::GetKey("qos_dds_max_instances", QVariant::String);
        data_list.append(new Data(key, "LENGTH_UNLIMITED"));
    }
    {
        //qos_dds_max_samples_per_instance
        auto key = Key::GetKey("qos_dds_max_samples_per_instance", QVariant::String);
        data_list.append(new Data(key, "LENGTH_UNLIMITED"));
    }

    return data_list;
}
