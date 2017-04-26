#include "dds_writerdatalifecycleqospolicy.h"
#include "../../../data.h"

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy():Node(NK_QOS_DDS_POLICY_WRITERDATALIFECYCLE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_WriterDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_WriterDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_WriterDataLifecycleQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "writer_data_lifecycle");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        auto key = Key::GetKey("qos_dds_autodispose_unregistered_instances", QVariant::Bool);
        data_list.append(new Data(key, true));
    }

    return data_list;


}
