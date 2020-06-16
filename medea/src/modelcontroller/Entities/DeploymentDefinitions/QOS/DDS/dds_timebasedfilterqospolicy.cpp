#include "dds_timebasedfilterqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER;
const static QString kind_string = "DDS_TimeBasedFilterQosPolicy";

void DDS_TimeBasedFilterQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_TimeBasedFilterQosPolicy(broker, is_temp_node);
    });
}

DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "time_based_filter");
    broker.AttachData(this, KeyName::QosDdsMinSeparationSec, QVariant::String, ProtectedState::UNPROTECTED,  "0");
    broker.AttachData(this, KeyName::QosDdsMinSeparationNanoSec, QVariant::String, ProtectedState::UNPROTECTED,  "0");
}