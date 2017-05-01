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

Edge *EntityFactory::createEdge(Node *source, Node *destination, EDGE_KIND edge_kind)
{
    return _createEdge(source, destination, edge_kind);
}


QString EntityFactory::getNodeKindString(NODE_KIND kind)
{
    QString kind_str;
    auto node_struct = globalFactory()->getNodeStruct(kind);
    if(node_struct){
        kind_str = node_struct->kind_str;
    }
    return kind_str;
}

QString EntityFactory::getEdgeKindString(EDGE_KIND kind)
{
    QString kind_str;
    auto edge_struct = globalFactory()->getEdgeStruct(kind);
    if(edge_struct){
        kind_str = edge_struct->kind_str;
    }
    return kind_str;
}

QList<NODE_KIND> EntityFactory::getNodeKinds()
{
    return globalFactory()->node_struct_lookup.keys();
}

QList<EDGE_KIND> EntityFactory::getEdgeKinds()
{
    return globalFactory()->edge_struct_lookup.keys();
}


NODE_KIND EntityFactory::getNodeKind(QString kind)
{
    return globalFactory()->node_kind_lookup.value(kind, NODE_KIND::NONE);
}

EDGE_KIND EntityFactory::getEdgeKind(QString kind)
{
    return globalFactory()->edge_kind_lookup.value(kind, EDGE_KIND::NONE);
}

void EntityFactory::RegisterNodeKind(NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor){
    //TODO
    auto node = getNodeStruct(kind);

    if(node){
        node->kind = kind;
        node->kind_str = kind_string;
        node->constructor = constructor;

        //Insert into our reverse lookup
        if(!node_kind_lookup.contains(kind_string)){
            qCritical() << "Lookup: " << kind_string << " -> " << (int)kind;
            node_kind_lookup.insert(kind_string, kind);
        }
    
        qCritical() << "Registered Node: " << (int)kind << " " << kind_string << " " << (constructor != 0);
    }
}

void EntityFactory::RegisterEdgeKind(EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor){
    auto edge = getEdgeStruct(kind);
    if(edge){
        edge->kind = kind;
        edge->kind_str = kind_string;
        edge->constructor = constructor;

        //Insert into our reverse lookup
        if(!edge_kind_lookup.contains(kind_string)){
            qCritical() << "Lookup: " << kind_string << " -> " << (int)kind;
            edge_kind_lookup.insert(kind_string, kind);
        }
        qCritical() << "Registered Edge: " << (int)kind << " " << kind_string << " " << (constructor != 0);
    }
}

void EntityFactory::RegisterDefaultData(EDGE_KIND kind, QString key_name, QVariant::Type type, bool is_protected, QVariant value){
    auto edge_struct = getEdgeStruct(kind);
    if(edge_struct){
        auto data = edge_struct->default_data.value(key_name, 0);
        if(!data){
            //Construct and insert a new default data
            data = new DefaultDataStruct();
            data->key_name = key_name;
            data->type = type;
            //Insert
            edge_struct->default_data.insert(key_name, data);
        }

        //Update the default data map
        if(data){
            //Update
            data->is_protected = is_protected;
            data->value = value;
        }
    }
}


void EntityFactory::RegisterDefaultData(NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected, QVariant value){
    auto node = getNodeStruct(kind);

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

    //Edges
    DefinitionEdge(this);
    AggregateEdge(this);
    WorkflowEdge(this);
    AssemblyEdge(this);
    DataEdge(this);
    DeploymentEdge(this);
    QosEdge(this);
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

EntityFactory::NodeLookupStruct* EntityFactory::getNodeStruct(NODE_KIND kind){
    auto node = node_struct_lookup.value(kind, 0);
    if(!node){
        qCritical() << "REGISTERED NODE KIND: " << (int)kind;
        node = new NodeLookupStruct();
        node->kind = kind;
        node_struct_lookup.insert(kind, node);
    }
    return node;
}

EntityFactory::EdgeLookupStruct* EntityFactory::getEdgeStruct(EDGE_KIND kind){
    auto edge = edge_struct_lookup.value(kind, 0);
    if(!edge){
        edge = new EdgeLookupStruct();
        edge->kind = kind;
        edge_struct_lookup.insert(kind, edge);
    }
    return edge;
}



QList<Data *> EntityFactory::getDefaultNodeData(NODE_KIND kind)
{
    QList<Data*> data_list;
    auto node = getNodeStruct(kind);
    if(node){
        data_list = getDefaultData(node->default_data.values());
    }
    return data_list;
}

QList<Data *> EntityFactory::getDefaultEdgeData(EDGE_KIND kind)
{
    QList<Data*> data_list;
    auto edge = getEdgeStruct(kind);
    if(edge){
        data_list = getDefaultData(edge->default_data.values());
    }
    return data_list;
}

QList<Data*> EntityFactory::getDefaultData(QList<DefaultDataStruct*> data){
    QList<Data*> data_list;
    foreach(auto data_struct, data){
        auto key = GetKey(data_struct->key_name, data_struct->type);
        auto data = new Data(key, data_struct->value, data_struct->is_protected);
        data_list << data;
    }
    return data_list;
}

Edge *EntityFactory::_createEdge(Node *source, Node *destination, EDGE_KIND kind)
{
    Edge* edge = 0;

    auto edge_struct = getEdgeStruct(kind);
    if(edge_struct && edge_struct->constructor){
        edge = edge_struct->constructor(source, destination);
        if(edge){
            auto data = getDefaultData(edge_struct->default_data.values());
            edge->addData(data);
        }
    }

    if(!edge){
        qCritical() << "Edge Kind: " << getEdgeKindString(kind) << " Not Implemented!";
    }
    return edge;
}

Node *EntityFactory::_createNode(NODE_KIND kind)
{
    Node* node = 0;
    auto node_struct = getNodeStruct(kind);

    if(node_struct && node_struct->constructor){
        node = node_struct->constructor();
        if(node){
            auto data = getDefaultData(node_struct->default_data.values());
            node->addData(data);
        }
    }

    if(!node){
        qCritical() << "Node Kind: " << getNodeKindString(kind) << " Not Implemented!";
    }

    return node;
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
        }else if(key_name == "index"){
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