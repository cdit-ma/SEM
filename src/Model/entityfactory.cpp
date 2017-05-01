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
    qCritical() << "Trying to create Node: " << nodeKind;
    return createNode(getNodeKind(nodeKind));
}

Node *EntityFactory::createNode(NODE_KIND nodeKind)
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


QString EntityFactory::getNodeKindString(NODE_KIND nodeKind)
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

QList<NODE_KIND> EntityFactory::getNodeKinds()
{
    return globalFactory()->nodeLookup.keys();
}

QList<Edge::EDGE_KIND> EntityFactory::getEdgeKinds()
{
    return globalFactory()->edgeLookup.keys();
}


NODE_KIND EntityFactory::getNodeKind(QString kind)
{
    return globalFactory()->nodeKindLookup.value(kind, NODE_KIND::NONE);
}

Edge::EDGE_KIND EntityFactory::getEdgeKind(QString kind)
{
    return globalFactory()->edgeKindLookup.value(kind, Edge::EC_NONE);
}

void EntityFactory::RegisterNodeKind(NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor){
    //TODO
    auto node = getNodeLookup(kind);

    if(node){
        node->kind = kind;
        node->kind_str = kind_string;
        node->constructor = constructor;

        //Insert into our reverse lookup
        if(!nodeKindLookup.contains(kind_string)){
            qCritical() << "Lookup: " << kind_string << " -> " << (int)kind;
            nodeKindLookup.insert(kind_string, kind);
        }
    
        qCritical() << "Registered Node: " << (int)kind << " " << kind_string << " " << (constructor != 0);
    }
}

void EntityFactory::RegisterDefaultData(NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected, QVariant value){
    
    //TODO
    auto node = getNodeLookup(kind);

    if(node){
        auto data = node->default_data.value(key_name, 0);
        if(!data){
            //Construct and insert a new default data
            data = new DefaultDataStruct();
            data->key_name = key_name;
            data->type = type;
            //Insert
            node->default_data.insert(key_name, data);
        }

        //Update the default data map
        if(data){
            //Update
            data->is_protected = is_protected;
            data->value = value;
        }
    }else{
        qCritical() << "NO NODE!";
    }
}
void EntityFactory::RegisterValidDataValues(NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values){
    //Register the valid values to the key
    auto key = GetKey(key_name, type);
    if(key){
        //Register the values
        key->addValidValues(values, kind);
    }
}   
EntityFactory::EntityFactory()
{
    //Aspects
    Model(this);
    BehaviourDefinitions(this);
    AssemblyDefinitions(this);
    DeploymentDefinitions(this);
    HardwareDefinitions(this);
    WorkerDefinitions(this);
    InterfaceDefinitions(this);

    //Impl Elements
    ComponentImpl(this);
    AttributeImpl(this);
    InEventPortImpl(this);
    OutEventPortImpl(this);

    //Behaviour Elements
    BranchState(this);
    Code(this);
    Condition(this);
    ForCondition(this);
    Header(this);
    InputParameter(this);
    PeriodicEvent(this);
    Process(this);
    ReturnParameter(this);
    Setter(this);
    Termination(this);
    Variable(this);
    VariableParameter(this);
    VariadicParameter(this);
    WhileLoop(this);
    WorkerProcess(this);
    Workload(this);

    //Instance Elements
    ComponentInstance(this);
    AttributeInstance(this);
    InEventPortInstance(this);
    OutEventPortInstance(this);
    BlackBoxInstance(this);
    AggregateInstance(this);
    MemberInstance(this);
    VectorInstance(this);
    
    //Deployment Elements
    ComponentAssembly(this);
    HardwareNode(this);
    HardwareCluster(this);
    ManagementComponent(this);
    InEventPortDelegate(this);
    OutEventPortDelegate(this);
    LoggingProfile(this);
    LoggingServer(this);

    //Definition Elements
    Aggregate(this);
    Attribute(this);
    BlackBox(this);
    Component(this);
    InEventPort(this);
    Member(this);
    OutEventPort(this);
    Vector(this);

    //QOS Profiles
    DDS_QOSProfile(this);
    DDS_DeadlineQosPolicy(this);
    DDS_DestinationOrderQosPolicy(this);
    DDS_DurabilityQosPolicy(this);
    DDS_DurabilityServiceQosPolicy(this);
    DDS_EntityFactoryQosPolicy(this);
    DDS_GroupDataQosPolicy(this);
    DDS_HistoryQosPolicy(this);
    DDS_LatencyBudgetQosPolicy(this);
    DDS_LifespanQosPolicy(this);
    DDS_LivelinessQosPolicy(this);
    DDS_OwnershipQosPolicy(this);
    DDS_OwnershipStrengthQosPolicy(this);
    DDS_PartitionQosPolicy(this);
    DDS_PresentationQosPolicy(this);
    DDS_ReaderDataLifecycleQosPolicy(this);
    DDS_ReliabilityQosPolicy(this);
    DDS_ResourceLimitsQosPolicy(this);
    DDS_TimeBasedFilterQosPolicy(this);
    DDS_TopicDataQosPolicy(this);
    DDS_TransportPriorityQosPolicy(this);
    DDS_UserDataQosPolicy(this);
    DDS_WriterDataLifecycleQosPolicy(this);

    //Elements
    
    IDL(this);


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

EntityFactory::NodeLookupStruct* EntityFactory::getNodeLookup(NODE_KIND kind){
    auto node = nodeLookup.value(kind, 0);
    if(!node){
        qCritical() << "REGISTERED NODE KIND: " << (int)kind;
        node = new NodeLookupStruct();
        node->kind = kind;
        nodeLookup.insert(kind, node);
    }
    return node;
}

void EntityFactory::setupOnceOffKeys(Node* node){
    if(node){
        //Construct the keys
        foreach(auto data_struct, node->getDefaultDataStructs()){
            auto key = GetKey(data_struct->key_name, data_struct->type);
        }
        for(auto key_name : node->getValidValueKeys()){
            qCritical() << "Got Valid Value for: " << key_name;
            auto key = GetKey(key_name);
            if(key){
                auto values = node->getValidValues(key_name);
                key->addValidValues(values, node->getNodeKind());
            }
        }
    }
}

Node *EntityFactory::_createNode(NODE_KIND kind)
{
    Node* node = 0;

    auto nS = nodeLookup.value(kind, 0);

    if(nS && nS->constructor){
        node = nS->constructor();
    }else{
        qCritical() << "Can't find lookup for: " << (int) kind;
    }
    
    if(!node){
        //qCritical() << "Node Kind: " << kind << " Not Implemented!";
        qCritical() << "Node Kind: " << getNodeKindString(kind) << " Not Implemented!";
    }else{
        //Attach data
        auto data = _constructDefaultData(node);
        node->addData(data);
    }
    return node;
}

QList<Data *> EntityFactory::getDefaultNodeData(NODE_KIND kind)
{
    QList<Data*> data_list;
    auto node = getNodeLookup(kind);
    if(node){
        foreach(auto data_struct, node->default_data){
            auto key = GetKey(data_struct->key_name, data_struct->type);
            auto data = new Data(key, data_struct->value, data_struct->is_protected);
            data_list << data;
        }
    }
    qCritical() << "DATA: " << data_list.size();
    return data_list;
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
            key = new Key(key_name, type);
        }
        keyLookup_[key_name] = key;
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