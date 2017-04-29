#include "entityfactory.h"
#include "key.h"
#include "data.h"

#include "Keys/labelkey.h"
#include "Keys/indexkey.h"
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
#include "DeploymentDefinitions/loggingserver.h"

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

//Edges
#include "Edges/aggregateedge.h"
#include "Edges/assemblyedge.h"
#include "Edges/dataedge.h"
#include "Edges/definitionedge.h"
#include "Edges/deploymentedge.h"
#include "Edges/qosedge.h"
#include "Edges/workflowedge.h"

EntityFactory* EntityFactory::global_factory = 0;



Node *EntityFactory::createNode(QString nodeKind)
{
    return createNode(getNodeKind(nodeKind));
}

Node *EntityFactory::createNode(Node::NODE_KIND nodeKind)
{
    return _createNode(nodeKind);
}

Edge *EntityFactory::createEdge(Node *source, Node *destination, QString edge_kind)
{
    return createEdge(source, destination, getEdgeKind(edge_kind));
}

Edge *EntityFactory::createEdge(Node *source, Node *destination, Edge::EDGE_KIND edge_kind)
{
    return _createEdge(source, destination, edge_kind);
}


QString EntityFactory::getNodeKindString(Node::NODE_KIND nodeKind)
{
    QString kind;
    auto nS = globalFactory()->nodeLookup.value(nodeKind, 0);
    if(nS){
        kind = nS->kind_str;
    }
    return kind;
}

QString EntityFactory::getEdgeKindString(Edge::EDGE_KIND edgeKind)
{
    QString kind;
    auto eS = globalFactory()->edgeLookup.value(edgeKind, 0);
    if(eS){
        kind = eS->kind_str;
    }
    return kind;
}

QList<Node::NODE_KIND> EntityFactory::getNodeKinds()
{
    return globalFactory()->nodeLookup.keys();
}

QList<Edge::EDGE_KIND> EntityFactory::getEdgeKinds()
{
    return globalFactory()->edgeLookup.keys();
}


Node::NODE_KIND EntityFactory::getNodeKind(QString kind)
{
    return globalFactory()->nodeKindLookup.value(kind, Node::NK_NONE);
}

Edge::EDGE_KIND EntityFactory::getEdgeKind(QString kind)
{
    return globalFactory()->edgeKindLookup.value(kind, Edge::EC_NONE);
}


EntityFactory::EntityFactory()
{
    //Aspects
    addNodeKind(Node::NK_MODEL, "Model", [](){return new Model();});
    addNodeKind(Node::NK_BEHAVIOUR_DEFINITIONS, "BehaviourDefinitions", [](){return new BehaviourDefinitions();});
    addNodeKind(Node::NK_ASSEMBLY_DEFINITIONS, "AssemblyDefinitions", [](){return new AssemblyDefinitions();});
    addNodeKind(Node::NK_DEPLOYMENT_DEFINITIONS, "DeploymentDefinitions", [](){return new DeploymentDefinitions();});
    addNodeKind(Node::NK_HARDWARE_DEFINITIONS, "HardwareDefinitions", [](){return new HardwareDefinitions();});
    addNodeKind(Node::NK_WORKER_DEFINITIONS, "WorkerDefinitions", [](){return new WorkerDefinitions();});
    addNodeKind(Node::NK_INTERFACE_DEFINITIONS, "InterfaceDefinitions", [](){return new InterfaceDefinitions();});

    //Impl Elements
    addNodeKind(Node::NK_COMPONENT_IMPL, "ComponentImpl", [](){return new ComponentImpl();});
    addNodeKind(Node::NK_ATTRIBUTE_IMPL, "AttributeImpl", [](){return new AttributeImpl();});
    addNodeKind(Node::NK_INEVENTPORT_IMPL, "InEventPortImpl", [](){return new InEventPortImpl();});
    addNodeKind(Node::NK_OUTEVENTPORT_IMPL, "OutEventPortImpl", [](){return new OutEventPortImpl();});

    //Behaviour Elements
    addNodeKind(Node::NK_BRANCH_STATE, "BranchState", [](){return new BranchState();});
    addNodeKind(Node::NK_CODE, "Code", [](){return new Code();});
    addNodeKind(Node::NK_CONDITION, "Condition", [](){return new Condition();});
    addNodeKind(Node::NK_FOR_CONDITION, "ForCondition", [](){return new ForCondition();});
    addNodeKind(Node::NK_HEADER, "Header", [](){return new Header();});
    addNodeKind(Node::NK_INPUT_PARAMETER, "InputParameter", [](){return new InputParameter();});
    addNodeKind(Node::NK_PERIODICEVENT, "PeriodicEvent", [](){return new PeriodicEvent();});
    addNodeKind(Node::NK_PROCESS, "Process", [](){return new Process();});
    addNodeKind(Node::NK_RETURN_PARAMETER, "ReturnParameter", [](){return new ReturnParameter();});
    addNodeKind(Node::NK_SETTER, "Setter", [](){return new Setter();});
    addNodeKind(Node::NK_TERMINATION, "Termination", [](){return new Termination();});
    addNodeKind(Node::NK_VARIABLE, "Variable", [](){return new Variable();});
    addNodeKind(Node::NK_VARIABLE_PARAMETER, "VariableParameter", [](){return new VariableParameter();});
    addNodeKind(Node::NK_VARIADIC_PARAMETER, "VariadicParameter", [](){return new VariadicParameter();});
    addNodeKind(Node::NK_WHILELOOP, "WhileLoop", [](){return new WhileLoop();});
    addNodeKind(Node::NK_WORKER_PROCESS, "WorkerProcess", [](){return new WorkerProcess();});
    addNodeKind(Node::NK_WORKLOAD, "Workload", [](){return new Workload();});

    //Instance Elements
    addNodeKind(Node::NK_COMPONENT_INSTANCE, "ComponentInstance", [](){return new ComponentInstance();});
    addNodeKind(Node::NK_ATTRIBUTE_INSTANCE, "AttributeInstance", [](){return new AttributeInstance();});
    addNodeKind(Node::NK_INEVENTPORT_INSTANCE, "InEventPortInstance", [](){return new InEventPortInstance();});
    addNodeKind(Node::NK_OUTEVENTPORT_INSTANCE, "OutEventPortInstance", [](){return new OutEventPortInstance();});
    addNodeKind(Node::NK_BLACKBOX_INSTANCE, "BlackBoxInstance", [](){return new BlackBoxInstance();});
    addNodeKind(Node::NK_AGGREGATE_INSTANCE, "AggregateInstance", [](){return new AggregateInstance();});
    addNodeKind(Node::NK_MEMBER_INSTANCE, "MemberInstance", [](){return new MemberInstance();});
    addNodeKind(Node::NK_VECTOR_INSTANCE, "VectorInstance", [](){return new VectorInstance();});

    //Deployment Elements
    addNodeKind(Node::NK_COMPONENT_ASSEMBLY, "ComponentAssembly", [](){return new ComponentAssembly();});
    addNodeKind(Node::NK_HARDWARE_NODE, "HardwareNode", [](){return new HardwareNode();});
    addNodeKind(Node::NK_HARDWARE_CLUSTER, "HardwareCluster", [](){return new HardwareCluster();});
    addNodeKind(Node::NK_MANAGEMENT_COMPONENT, "ManagementComponent", [](){return new ManagementComponent();});
    addNodeKind(Node::NK_INEVENTPORT_DELEGATE, "InEventPortDelegate", [](){return new InEventPortDelegate();});
    addNodeKind(Node::NK_OUTEVENTPORT_DELEGATE, "OutEventPortDelegate", [](){return new OutEventPortDelegate();});
    addNodeKind(Node::NK_LOGGINGPROFILE, "LoggingProfile", [](){return new LoggingProfile();});
    addNodeKind(Node::NK_LOGGINGSERVER, "LoggingServer", [](){return new LoggingServer();});


    //Definition Elements
    addNodeKind(Node::NK_AGGREGATE, "Aggregate", [](){return new Aggregate();});
    addNodeKind(Node::NK_ATTRIBUTE, "Attribute", [](){return new Attribute();});
    addNodeKind(Node::NK_BLACKBOX, "BlackBox", [](){return new BlackBox();});
    addNodeKind(Node::NK_COMPONENT, "Component", [](){return new Component();});
    addNodeKind(Node::NK_INEVENTPORT, "InEventPort", [](){return new InEventPort();});
    addNodeKind(Node::NK_MEMBER, "Member", [](){return new Member();});
    addNodeKind(Node::NK_OUTEVENTPORT, "OutEventPort", [](){return new OutEventPort();});
    addNodeKind(Node::NK_VECTOR, "Vector", [](){return new Vector();});

    //QOS Profiles
    addNodeKind(Node::NK_QOS_DDS_PROFILE, "DDS_QOSProfile", [](){return new DDS_QOSProfile();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_DEADLINE, "DDS_DeadlineQosPolicy", [](){return new DDS_DeadlineQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_DESTINATIONORDER, "DDS_DestinationOrderQosPolicy", [](){return new DDS_DestinationOrderQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_DURABILITY, "DDS_DurabilityQosPolicy", [](){return new DDS_DurabilityQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_DURABILITYSERVICE, "DDS_DurabilityServiceQosPolicy", [](){return new DDS_DurabilityServiceQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_ENTITYFACTORY, "DDS_EntityFactoryQosPolicy", [](){return new DDS_EntityFactoryQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_GROUPDATA, "DDS_GroupDataQosPolicy", [](){return new DDS_GroupDataQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_HISTORY, "DDS_HistoryQosPolicy", [](){return new DDS_HistoryQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_LATENCYBUDGET, "DDS_LatencyBudgetQosPolicy", [](){return new DDS_LatencyBudgetQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_LIFESPAN, "DDS_LifespanQosPolicy", [](){return new DDS_LifespanQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_LIVELINESS, "DDS_LivelinessQosPolicy", [](){return new DDS_LivelinessQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_OWNERSHIP, "DDS_OwnershipQosPolicy", [](){return new DDS_OwnershipQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_OWNERSHIPSTRENGTH, "DDS_OwnershipStrengthQosPolicy", [](){return new DDS_OwnershipStrengthQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_PARTITION, "DDS_PartitionQosPolicy", [](){return new DDS_PartitionQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_PRESENTATION, "DDS_PresentationQosPolicy", [](){return new DDS_PresentationQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_READERDATALIFECYCLE, "DDS_ReaderDataLifecycleQosPolicy", [](){return new DDS_ReaderDataLifecycleQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_RELIABILITY, "DDS_ReliabilityQosPolicy", [](){return new DDS_ReliabilityQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_RESOURCELIMITS, "DDS_ResourceLimitsQosPolicy", [](){return new DDS_ResourceLimitsQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_TIMEBASEDFILTER, "DDS_TimeBasedFilterQosPolicy", [](){return new DDS_TimeBasedFilterQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_TOPICDATA, "DDS_TopicDataQosPolicy", [](){return new DDS_TopicDataQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_TRANSPORTPRIORITY, "DDS_TransportPriorityQosPolicy", [](){return new DDS_TransportPriorityQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_USERDATA, "DDS_UserDataQosPolicy", [](){return new DDS_UserDataQosPolicy();});
    addNodeKind(Node::NK_QOS_DDS_POLICY_WRITERDATALIFECYCLE, "DDS_WriterDataLifecycleQosPolicy", [](){return new DDS_WriterDataLifecycleQosPolicy();});

    //Elements
    addNodeKind(Node::NK_IDL, "IDL", [](){return new IDL();});
    addNodeKind(Node::NK_NONE, "NO KIND", [](){return (Node*)0;});

    addEdgeKind(Edge::EC_DEFINITION, "Edge_Definition", &DefinitionEdge::createDefinitionEdge);
    addEdgeKind(Edge::EC_AGGREGATE, "Edge_Aggregate", &AggregateEdge::createAggregateEdge);
    addEdgeKind(Edge::EC_WORKFLOW, "Edge_Workflow", &WorkflowEdge::createWorkflowEdge);
    addEdgeKind(Edge::EC_ASSEMBLY, "Edge_Assembly", &AssemblyEdge::createAssemblyEdge);
    addEdgeKind(Edge::EC_DATA, "Edge_Data", &DataEdge::createDataEdge);
    addEdgeKind(Edge::EC_DEPLOYMENT, "Edge_Deployment", &DeploymentEdge::createDeploymentEdge);
    addEdgeKind(Edge::EC_QOS, "Edge_QOS", &QOSEdge::createQOSEdge);
}

EntityFactory::~EntityFactory()
{
    //TODO: Teardown objects
}

EntityFactory *EntityFactory::globalFactory()
{
    if(global_factory == 0){
        global_factory = new EntityFactory();
    }
    return global_factory;
}

EntityFactory *EntityFactory::getNewFactory()
{
    return new EntityFactory();
}

Node *EntityFactory::_createNode(Node::NODE_KIND kind)
{
    Node* node = 0;

    auto nS = nodeLookup.value(kind, 0);

    if(nS){
        node = nS->constructor();
    }
    
    if(!node){
        qCritical() << "Node Kind: " << kind << " Not Implemented!";
        qCritical() << "Node Kind: " << getNodeKindString(kind) << " Not Implemented!";
    }else{
        //Attach data
        auto data = _constructDefaultData(node);
        node->addData(data);
    }
    return node;
}

QList<Data *> EntityFactory::getDefaultNodeData(Node::NODE_KIND kind)
{
    return _constructDefaultData(getNode(kind));
}

Edge* EntityFactory::getEdge(Edge::EDGE_KIND kind){
    Edge* edge = 0;
    auto lookup = edgeLookup.value(kind, 0);
    if(lookup){
        if(!lookup->edge){
            //Construct a edge
            lookup->edge = lookup->constructor(0, 0);
        }
        edge = lookup->edge;
    }
    return edge;
}
Node* EntityFactory::getNode(Node::NODE_KIND kind){
    Node* node = 0;
    auto lookup = nodeLookup.value(kind, 0);
    if(lookup){
        if(!lookup->node){
            //Construct a node
            lookup->node = lookup->constructor();
        }
        node = lookup->node;
    }
    return node;
}

QList<Data *> EntityFactory::getDefaultEdgeData(Edge::EDGE_KIND kind)
{
    return _constructDefaultData(getEdge(kind));
}

QList<Data*> EntityFactory::_constructDefaultData(Entity* entity){
    QList<Data*> data_list;
    if(entity){
        foreach(auto data_struct, entity->getDefaultDataStructs()){
            auto key = GetKey(data_struct->key_name, data_struct->type);
            auto data = new Data(key, data_struct->value, data_struct->is_protected);
            data_list << data;
        }
        if(entity->isMoveEnabled()){
            auto x = GetKey("x", QVariant::Double);
            auto y = GetKey("y", QVariant::Double);
            data_list << new Data(x, -1);
            data_list << new Data(y, -1);
        }
        if(entity->isExpandEnabled()){
            auto x = GetKey("isExpanded", QVariant::Bool);
            data_list << new Data(x, true);
        }
    }   
    return data_list;
}

void EntityFactory::addNodeKind(Node::NODE_KIND kind, QString kind_str, std::function<Node *()> constructor)
{
    if(!nodeLookup.contains(kind)){
        auto nS = new NodeLookupStruct();
        nS->kind = kind;
        nS->kind_str = kind_str;
        nS->constructor = constructor;
        nS->node = 0;
        nodeLookup.insert(kind, nS);

        if(!nodeKindLookup.contains(kind_str)){
            nodeKindLookup.insert(kind_str, kind);
        }else{
            qCritical() << "Duplication: " << kind_str;
        }
    }else{
        qCritical() << "Duplication: " << kind;
    }
}

Edge *EntityFactory::_createEdge(Node *source, Node *destination, Edge::EDGE_KIND edgeKind)
{
    Edge* edge = 0;

    if(source && destination){
        auto eS = edgeLookup.value(edgeKind, 0);

        if(eS){
            edge = eS->constructor(source, destination);
        }
    }

    if(!edge){
        qCritical() << "Edge Kind: " << getEdgeKindString(edgeKind) << " Not Implemented!";
    }
    
    return edge;
}


Key *EntityFactory::GetKey(QString key_name)
{
    return keyLookup_.value(key_name, 0);
}

Key *EntityFactory::GetKey(int key_id)
{
    foreach(Key* key, keyLookup_.values()){
        if(key->getID() == key_id){
            return key;
        }
    }
    return 0;
}

Key *EntityFactory::GetKey(QString key_name, QVariant::Type type)
{
    if(keyLookup_.contains(key_name)){
        return keyLookup_[key_name];
    }else{
        Key* key = 0;
        if(key_name == "label"){
            key = new LabelKey();
        }else if(key_name == "sortOrder"){
            key = new IndexKey();    
        }else{
            key = new Key(key_name, type, Entity::EK_ALL);
        }
        return key;
    }
}


QList<Key*> EntityFactory::GetKeys(){
    return keyLookup_.values();
}

void EntityFactory::addEdgeKind(Edge::EDGE_KIND edge_kind, QString edge_kind_str, std::function<Edge *(Node *, Node *)> constructor)
{
    if(!edgeLookup.contains(edge_kind)){
        auto eS = new EdgeLookupStruct();
        eS->kind = edge_kind;
        eS->kind_str = edge_kind_str;
        eS->constructor = constructor;
        edgeLookup.insert(edge_kind, eS);

        if(!edgeKindLookup.contains(edge_kind_str)){
            edgeKindLookup.insert(edge_kind_str, edge_kind);
        }
    }
}