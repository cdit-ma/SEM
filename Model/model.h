#ifndef MODEL_H
#define MODEL_H

#include "edge.h"
#include "node.h"
#include "blanknode.h"
#include "key.h"
#include "data.h"



#include "BehaviourDefinitions/behaviourdefinitions.h"
#include "BehaviourDefinitions/componentimpl.h"
#include "BehaviourDefinitions/ineventportimpl.h"
#include "BehaviourDefinitions/outeventportimpl.h"
#include "BehaviourDefinitions/attributeimpl.h"

#include "BehaviourDefinitions/branchstate.h"
#include "BehaviourDefinitions/condition.h"
#include "BehaviourDefinitions/periodicevent.h"
#include "BehaviourDefinitions/process.h"
#include "BehaviourDefinitions/termination.h"
#include "BehaviourDefinitions/variable.h"
#include "BehaviourDefinitions/workload.h"
#include "BehaviourDefinitions/whileloop.h"




#include "BehaviourDefinitions/inputparameter.h"
#include "BehaviourDefinitions/returnparameter.h"

#include "BehaviourDefinitions/periodicevent.h"

#include "DeploymentDefinitions/deploymentdefinitions.h"
#include "DeploymentDefinitions/hardwaredefinitions.h"
#include "DeploymentDefinitions/assemblydefinitions.h"
#include "DeploymentDefinitions/deploymentdefinitions.h"


#include "DeploymentDefinitions/componentassembly.h"

#include "DeploymentDefinitions/attributeinstance.h"
#include "DeploymentDefinitions/componentinstance.h"
#include "DeploymentDefinitions/ineventportinstance.h"
#include "DeploymentDefinitions/outeventportinstance.h"

#include "DeploymentDefinitions/hardwarecluster.h"
#include "DeploymentDefinitions/hardwarenode.h"
#include "DeploymentDefinitions/managementcomponent.h"
#include "DeploymentDefinitions/ineventportdelegate.h"
#include "DeploymentDefinitions/outeventportdelegate.h"

#include "InterfaceDefinitions/interfacedefinitions.h"
#include "InterfaceDefinitions/aggregateinstance.h"

#include "InterfaceDefinitions/memberinstance.h"
#include "InterfaceDefinitions/attribute.h"
#include "InterfaceDefinitions/component.h"
#include "InterfaceDefinitions/outeventport.h"
#include "InterfaceDefinitions/ineventport.h"
#include "InterfaceDefinitions/idl.h"
#include "InterfaceDefinitions/aggregate.h"
#include "InterfaceDefinitions/member.h"
#include "InterfaceDefinitions/vector.h"
#include "InterfaceDefinitions/vectorinstance.h"

#include "InterfaceDefinitions/blackbox.h"
#include "DeploymentDefinitions/blackboxinstance.h"

#include "DeploymentDefinitions/QOS/qosprofile.h"

#include "DeploymentDefinitions/QOS/DDS/dds_qosprofile.h"

#include "DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.h"
#include "DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.h"


class Model: public Node
{
    Q_OBJECT
public:
    Model();
    ~Model();

    bool canAdoptChild(Node* child);
};
#endif // MODEL_H
