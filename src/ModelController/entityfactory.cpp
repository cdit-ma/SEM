#include "entityfactory.h"



#include <QDebug>


#include "Entities/key.h"
#include "Entities/data.h"

//Keys
#include "Entities/Keys/labelkey.h"
#include "Entities/Keys/indexkey.h"
#include "Entities/Keys/exportidkey.h"
#include "Entities/Keys/replicatecountkey.h"

//Model Includes

//Aspects
#include "Entities/model.h"
#include "Entities/workerdefinitions.h"
#include "Entities/BehaviourDefinitions/behaviourdefinitions.h"
#include "Entities/DeploymentDefinitions/assemblydefinitions.h"
#include "Entities/DeploymentDefinitions/deploymentdefinitions.h"
#include "Entities/DeploymentDefinitions/hardwaredefinitions.h"
#include "Entities/InterfaceDefinitions/interfacedefinitions.h"

//Impl Elements
#include "Entities/BehaviourDefinitions/componentimpl.h"
#include "Entities/BehaviourDefinitions/attributeimpl.h"
#include "Entities/BehaviourDefinitions/ineventportimpl.h"
#include "Entities/BehaviourDefinitions/outeventportimpl.h"

//Behaviour Elements
#include "Entities/BehaviourDefinitions/branchstate.h"
#include "Entities/BehaviourDefinitions/code.h"
#include "Entities/BehaviourDefinitions/condition.h"
#include "Entities/BehaviourDefinitions/forcondition.h"
#include "Entities/BehaviourDefinitions/header.h"
#include "Entities/BehaviourDefinitions/inputparameter.h"
#include "Entities/BehaviourDefinitions/periodicevent.h"
#include "Entities/BehaviourDefinitions/process.h"
#include "Entities/BehaviourDefinitions/returnparameter.h"
#include "Entities/BehaviourDefinitions/setter.h"
#include "Entities/BehaviourDefinitions/termination.h"
#include "Entities/BehaviourDefinitions/variable.h"
#include "Entities/BehaviourDefinitions/variableparameter.h"
#include "Entities/BehaviourDefinitions/variadicparameter.h"
#include "Entities/BehaviourDefinitions/whileloop.h"
#include "Entities/BehaviourDefinitions/workerprocess.h"
#include "Entities/BehaviourDefinitions/workload.h"

//Instance Elements
#include "Entities/DeploymentDefinitions/componentinstance.h"
#include "Entities/DeploymentDefinitions/attributeinstance.h"
#include "Entities/DeploymentDefinitions/ineventportinstance.h"
#include "Entities/DeploymentDefinitions/outeventportinstance.h"
#include "Entities/DeploymentDefinitions/blackboxinstance.h"
#include "Entities/InterfaceDefinitions/aggregateinstance.h"
#include "Entities/InterfaceDefinitions/memberinstance.h"
#include "Entities/InterfaceDefinitions/vectorinstance.h"

//Deployment Elements
#include "Entities/DeploymentDefinitions/componentassembly.h"
#include "Entities/DeploymentDefinitions/hardwarecluster.h"
#include "Entities/DeploymentDefinitions/hardwarenode.h"
#include "Entities/DeploymentDefinitions/managementcomponent.h"
#include "Entities/DeploymentDefinitions/ineventportdelegate.h"
#include "Entities/DeploymentDefinitions/outeventportdelegate.h"
#include "Entities/DeploymentDefinitions/loggingprofile.h"
#include "Entities/DeploymentDefinitions/loggingserver.h"

//Definition Elements
#include "Entities/InterfaceDefinitions/aggregate.h"
#include "Entities/InterfaceDefinitions/attribute.h"
#include "Entities/InterfaceDefinitions/blackbox.h"
#include "Entities/InterfaceDefinitions/component.h"
#include "Entities/InterfaceDefinitions/ineventport.h"
#include "Entities/InterfaceDefinitions/member.h"
#include "Entities/InterfaceDefinitions/outeventport.h"
#include "Entities/InterfaceDefinitions/vector.h"

//Elements
#include "Entities/InterfaceDefinitions/idl.h"
#include "Entities/InterfaceDefinitions/shareddatatypes.h"


//QOS Elements
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_qosprofile.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.h"

//Edges
#include "Entities/Edges/aggregateedge.h"
#include "Entities/Edges/assemblyedge.h"
#include "Entities/Edges/dataedge.h"
#include "Entities/Edges/definitionedge.h"
#include "Entities/Edges/deploymentedge.h"
#include "Entities/Edges/qosedge.h"
#include "Entities/Edges/workflowedge.h"

EntityFactory* EntityFactory::global_factory = 0;


Node *EntityFactory::CreateTempNode(NODE_KIND nodeKind)
{
    return _createNode(nodeKind, true);
}

Node *EntityFactory::CreateNode(NODE_KIND nodeKind, int id)
{
    return _createNode(nodeKind, false, id);
}

Edge *EntityFactory::CreateEdge(Node *source, Node *destination, EDGE_KIND edge_kind, int id)
{
    return _createEdge(source, destination, edge_kind, id);
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
            node_kind_lookup.insert(kind_string, kind);
        }
        
        qCritical() << "EntityFactory: Registered Node #" << node_kind_lookup.size() << kind_string;
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
            edge_kind_lookup.insert(kind_string, kind);
        }
        qCritical() << "EntityFactory: Registered Edge #" << edge_kind_lookup.size() << kind_string;
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

            if(value.isValid()){
                data->value = value;    
            }
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
            if(value.isValid()){
                data->value = value;    
            }
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
    SharedDatatypes(this);

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
        auto data = CreateData(key, data_struct->value, data_struct->is_protected);
        if(data){
            data_list << data;
        }
    }
    return data_list;
}

Edge *EntityFactory::_createEdge(Node *source, Node *destination, EDGE_KIND kind, int id)
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

    StoreEntity(edge, id);

    return edge;
}

void EntityFactory::StoreEntity(GraphML* graphml, int id){
    if(graphml){
        graphml->setFactory(this);
        RegisterEntity(graphml, id);
    }
}

Node *EntityFactory::_createNode(NODE_KIND kind, bool is_temporary, int id)
{
    Node* node = 0;
    auto node_struct = getNodeStruct(kind);

    if(node_struct && node_struct->constructor){
        node = node_struct->constructor();
        if(node && !is_temporary){
            auto data = getDefaultData(node_struct->default_data.values());
            node->addData(data);
        }
    }

    if(!node){
        qCritical() << "Node Kind: " << getNodeKindString(kind) << " Not Implemented!";
    }

    //Only store 
    if(!is_temporary){
        StoreEntity(node, id);
    }
    return node;
}


Key *EntityFactory::GetKey(QString key_name)
{
    return key_lookup_.value(key_name, 0);
}

Key *EntityFactory::GetKey(QString key_name, QVariant::Type type)
{
    if(key_lookup_.contains(key_name)){
        return key_lookup_[key_name];
    }else{
        Key* key = 0;
        if(key_name == "label"){
            key = new LabelKey();
        }else if(key_name == "index"){
            key = new IndexKey();    
        }else if(key_name == "uuid"){
            key = new ExportIDKey();
        }else if(key_name == "replicate_count"){
            key = new ReplicateCountKey();    
        }else{
            key = new Key(key_name, type);
        }
        key_lookup_[key_name] = key;
        StoreEntity(key);
        return key;
    }
}

QList<Key*> EntityFactory::GetKeys(){
    return key_lookup_.values();
}

void EntityFactory::DestructEntity(GraphML* graphml){
    if(graphml){
        //This will deregister
        delete graphml;
    }
}


GraphML* EntityFactory::getGraphML(int id){
    return hash_.value(id, 0);
}

Entity* EntityFactory::GetEntity(int id){
    auto item = getGraphML(id);
    if(item){
        switch(item->getGraphMLKind()){
            case GRAPHML_KIND::NODE:
            case GRAPHML_KIND::EDGE:
                return (Entity*) item;
            default:
                break;
        }
    }
    return 0;
}

Entity* EntityFactory::GetEntityByUUID(QString uuid){
    auto id = uuid_lookup_.value(uuid, -1);
    return GetEntity(id);
}

Node* EntityFactory::GetNode(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::NODE){
        return (Node*) item;
    }
    return 0;
}

Edge* EntityFactory::GetEdge(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::EDGE){
        return (Edge*) item;
    }
    return 0;
}

Data* EntityFactory::GetData(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::DATA){
        return (Data*) item;
    }
    return 0;
}

Key* EntityFactory::GetKey(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::KEY){
        return (Key*) item;
    }
    return 0;
}

Data* EntityFactory::CreateData(Key* key, QVariant value, bool is_protected){
    if(key){
        //Check if the key is in our list 
        auto data = new Data(key, value, is_protected);
        StoreEntity(data);
        return data;
    }
    return 0;
}
void EntityFactory::RegisterEntity(GraphML* graphml, int id){
    if(graphml){
        //If we haven't been given an id, or our hash contains our id already, we need to set a new one
        if(id == -1 || hash_.contains(id)){
            id = ++id_counter_;
        }

        //Get the id, post set
        id = graphml->setID(id);

        if(id > -1){
            if(!hash_.contains(id)){
                //qCritical() << "HASH: " << id << graphml->toString();
                hash_.insert(id, graphml);
            }else{
                qCritical() << graphml->toString() << ": HASH COLLISION @ " << id;
            }
        }
    }
}

void EntityFactory::DeregisterEntity(GraphML* graphml){
    if(graphml){
        auto id = graphml->getID();
        hash_.remove(id);
    }
}


void EntityFactory::EntityUUIDChanged(Entity* entity, QString uuid){
    if(entity){
        //Check for the old UUID
        auto old_uuid = entity->getDataValue("uuid").toString();
        uuid_lookup_.remove(old_uuid);

        if(!uuid.isEmpty()){
            uuid_lookup_.insert(uuid, entity->getID());
            qCritical() << "UUID[" << uuid_lookup_.size() << "]: " << uuid << " -> " << entity->toString();
        }
    }
}