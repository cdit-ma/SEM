#include "dds_writerdatalifecycleqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE;
const static QString kind_string = "DDS_WriterDataLifecycleQosPolicy";

void DDS_WriterDataLifecycleQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_WriterDataLifecycleQosPolicy(broker, is_temp_node);
    });
}

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "writer_data_lifecycle");
    broker.AttachData(this, KeyName::QosDdsAutoDisposeUnregisteredInstances, QVariant::Bool, ProtectedState::UNPROTECTED, true);
}