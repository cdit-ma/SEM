#include "nodefactory.h"
#include <QDebug>

//Model Includes

//Aspects
#include "model.h"
#include "workerdefinitions.h"
#include "BehaviourDefinitions/behaviourdefinitions.h"
#include "DeploymentDefinitions/assemblydefinitions.h"
#include "DeploymentDefinitions/deploymentdefinitions.h"
#include "DeploymentDefinitions/hardwaredefinitions.h"
#include "InterfaceDefinitions/interfacedefinitions.h"

//Impl Elements
#include "BehaviourDefinitions/componentimpl.h"
#include "BehaviourDefinitions/attributeimpl.h"
#include "BehaviourDefinitions/ineventportimpl.h"
#include "BehaviourDefinitions/outeventportimpl.h"

//Behaviour Elements
#include "BehaviourDefinitions/branchstate.h"
#include "BehaviourDefinitions/code.h"
#include "BehaviourDefinitions/condition.h"
#include "BehaviourDefinitions/forcondition.h"
#include "BehaviourDefinitions/header.h"
#include "BehaviourDefinitions/inputparameter.h"
#include "BehaviourDefinitions/periodicevent.h"
#include "BehaviourDefinitions/process.h"
#include "BehaviourDefinitions/returnparameter.h"
#include "BehaviourDefinitions/setter.h"
#include "BehaviourDefinitions/termination.h"
#include "BehaviourDefinitions/variable.h"
#include "BehaviourDefinitions/variableparameter.h"
#include "BehaviourDefinitions/variadicparameter.h"
#include "BehaviourDefinitions/whileloop.h"
#include "BehaviourDefinitions/workerprocess.h"
#include "BehaviourDefinitions/workload.h"

//Instance Elements
#include "DeploymentDefinitions/componentinstance.h"
#include "DeploymentDefinitions/attributeinstance.h"
#include "DeploymentDefinitions/ineventportinstance.h"
#include "DeploymentDefinitions/outeventportinstance.h"
#include "DeploymentDefinitions/blackboxinstance.h"
#include "InterfaceDefinitions/aggregateinstance.h"
#include "InterfaceDefinitions/memberinstance.h"
#include "InterfaceDefinitions/vectorinstance.h"

//Deployment Elements
#include "DeploymentDefinitions/componentassembly.h"
#include "DeploymentDefinitions/hardwarecluster.h"
#include "DeploymentDefinitions/hardwarenode.h"
#include "DeploymentDefinitions/managementcomponent.h"
#include "DeploymentDefinitions/ineventportdelegate.h"
#include "DeploymentDefinitions/outeventportdelegate.h"
#include "DeploymentDefinitions/loggingprofile.h"

//Definition Elements
#include "InterfaceDefinitions/aggregate.h"
#include "InterfaceDefinitions/attribute.h"
#include "InterfaceDefinitions/blackbox.h"
#include "InterfaceDefinitions/component.h"
#include "InterfaceDefinitions/ineventport.h"
#include "InterfaceDefinitions/member.h"
#include "InterfaceDefinitions/outeventport.h"
#include "InterfaceDefinitions/vector.h"

//Elements
#include "InterfaceDefinitions/idl.h"

//QOS Elements
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


NodeFactory* NodeFactory::factory = 0;



Node *NodeFactory::createNode(QString nodeKind)
{
    return createNode(getNodeKind(nodeKind));
}

Node *NodeFactory::createNode(Node::NODE_KIND nodeKind)
{
    return getFactory()->_createNode(nodeKind);
}

QString NodeFactory::getNodeKindString(Node::NODE_KIND nodeKind)
{
    QString kind;
    auto nS = getFactory()->nodeLookup.value(nodeKind, 0);
    if(nS){
        kind = nS->kind_str;
    }
    return kind;
}

QList<Node::NODE_KIND> NodeFactory::getNodeKinds()
{
    return getFactory()->nodeLookup.keys();
}

Node::NODE_KIND NodeFactory::getNodeKind(QString kind)
{
    return getFactory()->nodeKindLookup.value(kind, Node::NK_NONE);
}

NodeFactory::NodeFactory()
{
    //Aspects
    addKind(Node::NK_MODEL, "Model", [](){return new Model();});
    addKind(Node::NK_BEHAVIOUR_DEFINITIONS, "BehaviourDefinitions", [](){return new BehaviourDefinitions();});
    addKind(Node::NK_ASSEMBLY_DEFINITIONS, "AssemblyDefinitions", [](){return new AssemblyDefinitions();});
    addKind(Node::NK_DEPLOYMENT_DEFINITIONS, "DeploymentDefinitions", [](){return new DeploymentDefinitions();});
    addKind(Node::NK_HARDWARE_DEFINITIONS, "HardwareDefinitions", [](){return new HardwareDefinitions();});
    addKind(Node::NK_WORKER_DEFINITIONS, "WorkerDefinitions", [](){return new WorkerDefinitions();});
    addKind(Node::NK_INTERFACE_DEFINITIONS, "InterfaceDefinitions", [](){return new InterfaceDefinitions();});

    //Impl Elements
    addKind(Node::NK_COMPONENT_IMPL, "ComponentImpl", [](){return new ComponentImpl();});
    addKind(Node::NK_ATTRIBUTE_IMPL, "AttributeImpl", [](){return new AttributeImpl();});
    addKind(Node::NK_INEVENTPORT_IMPL, "InEventPortImpl", [](){return new InEventPortImpl();});
    addKind(Node::NK_OUTEVENTPORT_IMPL, "OutEventPortImpl", [](){return new OutEventPortImpl();});

    //Behaviour Elements
    addKind(Node::NK_BRANCH_STATE, "BranchState", [](){return new BranchState();});
    addKind(Node::NK_CODE, "Code", [](){return new Code();});
    addKind(Node::NK_CONDITION, "Condition", [](){return new Condition();});
    addKind(Node::NK_FOR_CONDITION, "ForCondition", [](){return new ForCondition();});
    addKind(Node::NK_HEADER, "Header", [](){return new Header();});
    addKind(Node::NK_INPUT_PARAMETER, "InputParameter", [](){return new InputParameter();});
    addKind(Node::NK_PERIODICEVENT, "PeriodicEvent", [](){return new PeriodicEvent();});
    addKind(Node::NK_PROCESS, "Process", [](){return new Process();});
    addKind(Node::NK_RETURN_PARAMETER, "ReturnParameter", [](){return new ReturnParameter();});
    addKind(Node::NK_SETTER, "Setter", [](){return new Setter();});
    addKind(Node::NK_TERMINATION, "Termination", [](){return new Termination();});
    addKind(Node::NK_VARIABLE, "Variable", [](){return new Variable();});
    addKind(Node::NK_VARIABLE_PARAMETER, "VariableParameter", [](){return new VariableParameter();});
    addKind(Node::NK_VARIADIC_PARAMETER, "VariadicParameter", [](){return new VariadicParameter();});
    addKind(Node::NK_WHILELOOP, "WhileLoop", [](){return new WhileLoop();});
    addKind(Node::NK_WORKER_PROCESS, "WorkerProcess", [](){return new WorkerProcess();});
    addKind(Node::NK_WORKLOAD, "Workload", [](){return new Workload();});

    //Instance Elements
    addKind(Node::NK_COMPONENT_INSTANCE, "ComponentInstance", [](){return new ComponentInstance();});
    addKind(Node::NK_ATTRIBUTE_INSTANCE, "AttributeInstance", [](){return new AttributeInstance();});
    addKind(Node::NK_INEVENTPORT_INSTANCE, "InEventPortInstance", [](){return new InEventPortInstance();});
    addKind(Node::NK_OUTEVENTPORT_INSTANCE, "OutEventPortInstance", [](){return new OutEventPortInstance();});
    addKind(Node::NK_BLACKBOX_INSTANCE, "BlackBoxInstance", [](){return new BlackBoxInstance();});
    addKind(Node::NK_AGGREGATE_INSTANCE, "AggregateInstance", [](){return new AggregateInstance();});
    addKind(Node::NK_MEMBER_INSTANCE, "MemberInstance", [](){return new MemberInstance();});
    addKind(Node::NK_VECTOR_INSTANCE, "VectorInstance", [](){return new VectorInstance();});

    //Deployment Elements
    addKind(Node::NK_COMPONENT_ASSEMBLY, "ComponentAssembly", [](){return new ComponentAssembly();});
    addKind(Node::NK_HARDWARE_NODE, "HardwareNode", [](){return new HardwareNode();});
    addKind(Node::NK_HARDWARE_CLUSTER, "HardwareCluster", [](){return new HardwareCluster();});
    addKind(Node::NK_MANAGEMENT_COMPONENT, "ManagementComponent", [](){return new ManagementComponent();});
    addKind(Node::NK_INEVENTPORT_DELEGATE, "InEventPortDelegate", [](){return new InEventPortDelegate();});
    addKind(Node::NK_OUTEVENTPORT_DELEGATE, "OutEventPortDelegate", [](){return new OutEventPortDelegate();});
    addKind(Node::NK_LOGGINGPROFILE, "LoggingProfile", [](){return new LoggingProfile();});


    //Definition Elements
    addKind(Node::NK_AGGREGATE, "Aggregate", [](){return new Aggregate();});
    addKind(Node::NK_ATTRIBUTE, "Attribute", [](){return new Attribute();});
    addKind(Node::NK_BLACKBOX, "BlackBox", [](){return new BlackBox();});
    addKind(Node::NK_COMPONENT, "Component", [](){return new Component();});
    addKind(Node::NK_INEVENTPORT, "InEventPort", [](){return new InEventPort();});
    addKind(Node::NK_MEMBER, "Member", [](){return new Member();});
    addKind(Node::NK_OUTEVENTPORT, "OutEventPort", [](){return new OutEventPort();});
    addKind(Node::NK_VECTOR, "Vector", [](){return new Vector();});

    //QOS Profiles
    addKind(Node::NK_QOS_DDS_PROFILE, "DDS_QOSProfile", [](){return new DDS_QOSProfile();});
    addKind(Node::NK_QOS_DDS_POLICY_DEADLINE, "DDS_DeadlineQosPolicy", [](){return new DDS_DeadlineQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_DESTINATIONORDER, "DDS_DestinationOrderQosPolicy", [](){return new DDS_DestinationOrderQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_DURABILITY, "DDS_DurabilityQosPolicy", [](){return new DDS_DurabilityQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_DURABILITYSERVICE, "DDS_DurabilityServiceQosPolicy", [](){return new DDS_DurabilityServiceQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_ENTITYFACTORY, "DDS_EntityFactoryQosPolicy", [](){return new DDS_EntityFactoryQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_GROUPDATA, "DDS_GroupDataQosPolicy", [](){return new DDS_GroupDataQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_HISTORY, "DDS_HistoryQosPolicy", [](){return new DDS_HistoryQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_LATENCYBUDGET, "DDS_LatencyBudgetQosPolicy", [](){return new DDS_LatencyBudgetQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_LIFESPAN, "DDS_LifespanQosPolicy", [](){return new DDS_LifespanQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_LIVELINESS, "DDS_LivelinessQosPolicy", [](){return new DDS_LivelinessQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_OWNERSHIP, "DDS_OwnershipQosPolicy", [](){return new DDS_OwnershipQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_OWNERSHIPSTRENGTH, "DDS_OwnershipStrengthQosPolicy", [](){return new DDS_OwnershipStrengthQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_PARTITION, "DDS_PartitionQosPolicy", [](){return new DDS_PartitionQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_PRESENTATION, "DDS_PresentationQosPolicy", [](){return new DDS_PresentationQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_READERDATALIFECYCLE, "DDS_ReaderDataLifecycleQosPolicy", [](){return new DDS_ReaderDataLifecycleQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_RELIABILITY, "DDS_ReliabilityQosPolicy", [](){return new DDS_ReliabilityQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_RESOURCELIMITS, "DDS_ResourceLimitsQosPolicy", [](){return new DDS_ResourceLimitsQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_TIMEBASEDFILTER, "DDS_TimeBasedFilterQosPolicy", [](){return new DDS_TimeBasedFilterQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_TOPICDATA, "DDS_TopicDataQosPolicy", [](){return new DDS_TopicDataQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_TRANSPORTPRIORITY, "DDS_TransportPriorityQosPolicy", [](){return new DDS_TransportPriorityQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_USERDATA, "DDS_UserDataQosPolicy", [](){return new DDS_UserDataQosPolicy();});
    addKind(Node::NK_QOS_DDS_POLICY_WRITERDATALIFECYCLE, "DDS_WriterDataLifecycleQosPolicy", [](){return new DDS_WriterDataLifecycleQosPolicy();});

    //Elements
    addKind(Node::NK_IDL, "IDL", [](){return new IDL();});
    addKind(Node::NK_NONE, "NO KIND", [](){return (Node*)0;});
}

NodeFactory *NodeFactory::getFactory()
{
    if(factory == 0){
        factory = new NodeFactory();
    }
    return factory;
}

Node *NodeFactory::_createNode(Node::NODE_KIND kind)
{
    Node* node = 0;

    auto nS = nodeLookup.value(kind, 0);

    if(nS){
        node = nS->constructor();

        if(!node){
            //qCritical() << "Node Kind: " << getNodeKindString(kind) << " Has no Constructor!";
        }
    }else{
        qCritical() << "Node Kind: " << getNodeKindString(kind) << " Not Implemented!";
    }

    return node;
}

void NodeFactory::addKind(Node::NODE_KIND kind, QString kind_str, std::function<Node *()> constructor)
{
    if(!nodeLookup.contains(kind)){
        auto nS = new NodeLookupStruct();
        nS->kind = kind;
        nS->kind_str = kind_str;
        nS->constructor = constructor;
        nodeLookup.insert(kind, nS);

        if(!nodeKindLookup.contains(kind_str)){
            nodeKindLookup.insert(kind_str, kind);
        }
    }
}
