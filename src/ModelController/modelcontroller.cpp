#include "modelcontroller.h"
#include <QDebug>
#include <algorithm>
#include <QDateTime>
#include <QSysInfo>
#include <QDir>
#include <QStringBuilder>
#include <QNetworkInterface>
#include <QThread>
#include <QFile>
#include <QElapsedTimer>
#include <QXmlStreamReader>

#include "../version.h"

#include "entityfactory.h"
#include "tempentity.h"

#include "Entities/edge.h"
#include "Entities/data.h"
#include "Entities/node.h"
#include "Entities/key.h"
#include "Entities/Keys/exportidkey.h"

#include "Entities/InterfaceDefinitions/eventport.h"
#include "Entities/InterfaceDefinitions/aggregate.h"
#include "Entities/InterfaceDefinitions/datanode.h"
#include "Entities/BehaviourDefinitions/workerprocess.h"
#include "Entities/DeploymentDefinitions/eventportdelegate.h"

inline QPair<bool, QString> readFile(QString filePath)
{
    QPair<bool, QString> result;

    QFile file(filePath);
    result.first = file.exists() && file.open(QFile::ReadOnly | QFile::Text);
    if (result.first) {
        QTextStream fileStream(&file);
        result.second = fileStream.readAll();
        file.close();
    }
    return result;
}

inline QString getXMLAttribute(QXmlStreamReader &xml, QString attribute)
{
    //Get the Attributes of the current XML entity.
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attribute)){
        return attributes.value(attribute).toString();
    }else{
        qCritical() << "Cannot find Attribute Key: " << attribute;
        return "";
    }
}

ModelController::ModelController():QObject(0)
{
    controller_thread = new QThread();
    moveToThread(controller_thread);

    connect(this, &ModelController::InitiateTeardown, this, &QObject::deleteLater, Qt::QueuedConnection);
    connect(this, &ModelController::InitiateTeardown, controller_thread, &QThread::quit, Qt::QueuedConnection);
    controller_thread->start();

    entity_factory = EntityFactory::getNewFactory();

    //Register meta types
    qRegisterMetaType<GRAPHML_KIND>("GRAPHML_KIND");
    qRegisterMetaType<EDGE_KIND>("EDGE_KIND");
    qRegisterMetaType<NODE_KIND>("NODE_KIND");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<MODEL_SEVERITY>("MODEL_SEVERITY");
}

void ModelController::SetupController(QString file_path)
{
    QWriteLocker lock(&lock_);
    setupModel();
    loadWorkerDefinitions();
    clearHistory();

    auto file = readFile(file_path);

    bool modified = true;
    if(file.first){
        setModelAction(MODEL_ACTION::OPEN);
        bool success = importGraphML(file.second, model);
        unsetModelAction(MODEL_ACTION::OPEN);
        
        if(success){
            //Update the project filePath
            setProjectPath(file_path);
        }

        modified = !success;
    }else{
        emit ProjectFileChanged("Untitled Project");
    }

    //Clear the Undo/Redo History.
    clearHistory();
    setProjectModified(modified);
    emit ModelReady(true);
}

ModelController::~ModelController()
{
    QList<Entity*> entities;
    entities << workerDefinitions << model;
    destructEntities(entities);
    delete entity_factory;
}

/**
 * @brief NewController::loadWorkerDefinitions Loads in both the compiled in WorkerDefinitions and Externally defined worker defintions.
 */
void ModelController::loadWorkerDefinitions()
{
    if(workerDefinitions){
        QList<QDir> worker_directories{QDir(":/WorkerDefinitions")};
        QStringList extensions{"*.worker"};

        setModelAction(MODEL_ACTION::IMPORT);
        for(auto dir : worker_directories){
            for(auto file : dir.entryList(extensions)){
                auto file_path = dir.absolutePath() + "/" + file;
                auto data = readFile(file_path);
                bool success = importGraphML(data.second, workerDefinitions);
                if(!success){
                    emit Notification(MODEL_SEVERITY::WARNING, "Error Importing Worker Definition '" + file_path + "'");
                }
            }
        }
        unsetModelAction(MODEL_ACTION::IMPORT);
    }
}

QString ModelController::exportGraphML(QList<int> ids, bool all_edges){
    return exportGraphML(getOrderedEntities(ids), all_edges);
}

QString ModelController::exportGraphML(Entity* entity){
    return exportGraphML(QList<Entity*>{entity}, true);
}

QString ModelController::exportGraphML(QList<Entity*> selection, bool all_edges){
    QSet<Key*> keys;

    QSet<Entity*> entities;
    QSet<Node*> top_nodes;
    QSet<Edge*> edges;
    
    //Get the entities
    for(auto entity : selection){
        //Insert into the list of entities
        entities.insert(entity);
        if(entity->isNode()){
            auto node = (Node*) entity;
            top_nodes.insert(node);

            //Add all the children to the list of entities (So we can check if we own all edges) 
            for(auto child : node->getChildren()){
                entities.insert(child);
            }

            //Add all of the edges
            for(auto edge : node->getAllEdges()){
                entities.insert(edge);
                edges.insert(edge);
            }
        }else if(entity->isEdge()){
            auto edge = (Edge*) entity;
            edges.insert(edge);
        }
    }

    //Get the Keys
    for(auto entity : entities){
        for(auto key : entity->getKeys()){
            keys.insert(key);
        }
    }

    QString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";
    //Define the key graphml
    for(auto key : keys){
        xml += key->toGraphML(1);
    }
    {
        xml +="\n\t<graph edgedefault=\"directed\" id=\"parentGraph0\">\n";
        for(auto entity : top_nodes){
            xml += entity->toGraphML(2);
        }
        
        for(auto edge : edges){
            auto src = edge->getSource();
            auto dst = edge->getDestination();
            //Only export the edge if we contain both sides, unless we should export all
            bool export_edge = all_edges;
            
            if(!export_edge){
                bool contains_src = entities.contains(src);
                bool contains_dst = entities.contains(dst);
                if(contains_src && contains_dst){
                    export_edge = true;
                }else{
                    //If we don't contain both
                    switch(edge->getEdgeKind()){
                        case EDGE_KIND::AGGREGATE:
                        case EDGE_KIND::ASSEMBLY:
                        case EDGE_KIND::DEPLOYMENT:{
                            export_edge = true;
                            break;
                        }
                        case EDGE_KIND::DEFINITION:{
                            export_edge = contains_src;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            if(export_edge){
                xml += edge->toGraphML(2);
            }
        }
        xml += "\t</graph>\n";
    }
    xml += "</graphml>";
    return xml;
}



int ModelController::GetEdgeOrderIndex(EDGE_KIND kind){
    switch(kind){
        case EDGE_KIND::DEFINITION:
            return 0;
        case EDGE_KIND::AGGREGATE:
            return 1;
        case EDGE_KIND::WORKFLOW:
            return 2;
        case EDGE_KIND::DATA:
            return 3;
        case EDGE_KIND::QOS:
            return 4;
        case EDGE_KIND::ASSEMBLY:
            return 5;
        case EDGE_KIND::DEPLOYMENT:
            return 6;
        default:
            return 100;
    }
}

QList<EDGE_KIND> ModelController::GetEdgeOrderIndexes(){
    QList<EDGE_KIND> indices;

    indices << EDGE_KIND::DEFINITION;
    indices << EDGE_KIND::AGGREGATE;
    indices << EDGE_KIND::WORKFLOW;
    indices << EDGE_KIND::DATA;
    indices << EDGE_KIND::QOS;
    indices << EDGE_KIND::ASSEMBLY;
    indices << EDGE_KIND::DEPLOYMENT;

    for(auto kind : entity_factory->getEdgeKinds()){
        if(!indices.contains(kind)){
            qCritical() << "Edge Kind: " << entity_factory->getEdgeKindString(kind) << "Not ordered";
            indices << kind;
        }
    }
    return indices;
}



bool ModelController::setData_(Entity *entity, QString key_name, QVariant value, bool add_action)
{
    if(entity){
        //Get an undo action
        auto action = getNewAction(GRAPHML_KIND::DATA);
        action.entity_id = entity->getID();
        action.Data.key_name = key_name;

        if(entity->gotData(key_name)){
            //Set the type
            action.Action.type = ACTION_TYPE::MODIFIED;
            //Store the old value
            action.Data.old_value = entity->getDataValue(key_name);
            //Update the new value
            entity->setDataValue(key_name, value);
        }else{
            //Set the type
            action.Action.type = ACTION_TYPE::CONSTRUCTED;
            auto key = entity_factory->GetKey(key_name, value.type());
            auto data = entity_factory->CreateData(key, value);
            
            if(data){
                //Attach the Data to the parent
                entity->addData(data);
            }
        }

        if(entity->gotData(key_name)){
            //Set the new value
            action.Data.new_value = entity->getDataValue(key_name);
            addActionToStack(action, true);
            return true;
        }
    }
    return false;
}

/**
 * @brief NewController::destructData - Removes and destroys a Data attached to an Entity
 * @param parent - The Entity the Data is attached to.
 * @param keyName - The Name of the Key of the Data
 * @param addAction - Add an undo state
 */
bool ModelController::destructData_(Entity* entity, QString key_name)
{  
    if(entity){
        //Get the data
        auto data = entity->getData(key_name);
        if(data){
            //Get an undo action
            auto action = getNewAction(GRAPHML_KIND::DATA);
            action.entity_id = entity->getID();
            action.Action.kind = GRAPHML_KIND::DATA;
            action.Action.type = ACTION_TYPE::DESTRUCTED;
            action.Data.key_name = key_name;
            action.Data.old_value = data->getValue();

            //Destroy the data
            entity_factory->DestructEntity(data);
            addActionToStack(action);
            return true;
        }
    }
    return false;
}


Node* ModelController::construct_temp_node(Node* parent_node, NODE_KIND node_kind){
    Node* node = 0;
    if(parent_node){
        //Construct node with default data
        node = entity_factory->CreateTempNode(node_kind);
        
        if(node){
            if(!parent_node->addChild(node)){
                //Free memory
                entity_factory->DestructEntity(node);
                node = 0;
            }
        }
    }
    return node;
}

Node* ModelController::check_for_existing_node(Node* parent_node, NODE_KIND node_kind){
    switch(node_kind){
        case NODE_KIND::MODEL:{
            return model;
        }
        case NODE_KIND::INTERFACE_DEFINITIONS:{
            return interfaceDefinitions;
        }
        case NODE_KIND::BEHAVIOUR_DEFINITIONS:{
            return behaviourDefinitions;
        }
        case NODE_KIND::WORKER_DEFINITIONS:{
            return workerDefinitions;
        }
        case NODE_KIND::ASSEMBLY_DEFINITIONS:{
            return assemblyDefinitions;
        }
        case NODE_KIND::DEPLOYMENT_DEFINITIONS:{
            return deploymentDefinitions;
        }
        case NODE_KIND::HARDWARE_DEFINITIONS:{
            return hardwareDefinitions;
        }
        default:
            return 0;
    }
}

Node* ModelController::construct_node(Node* parent_node, NODE_KIND node_kind, int id){
    Node* node = 0;
    node = check_for_existing_node(parent_node, node_kind);
    if(!node){ 
        //Construct node with default data
        node = entity_factory->CreateNode(node_kind, id);   
    }
    return node;
}

Node* ModelController::construct_child_node(Node* parent_node, NODE_KIND node_kind, bool notify_view){
    Node* node = 0;
    if(parent_node){
        //Don't construct nodes:
        node = construct_node(parent_node, node_kind);
        if(node){
            auto current_parent = node->getParentNode();

            //Check the current parent
            if(!current_parent){
                //If we don't have one set, try set the parent_node
                attachChildNode(parent_node, node, notify_view);
                current_parent = node->getParentNode();
            }

            //If we couldn't attach to the parent_node, we should remove 
            if(current_parent != parent_node){
                entity_factory->DestructEntity(node);
                //Free Memory
                node = 0;
            }
        }
    }
    return node;
}

Node* ModelController::construct_connected_node(Node* parent_node, NODE_KIND node_kind, Node* destination, EDGE_KIND edge_kind){
    Node* source = construct_child_node(parent_node, node_kind, false);
    if(source){
        auto edge = construct_edge(edge_kind, source, destination, -1, false);
        
        if(!edge){
            //Free the memory
            entity_factory->DestructEntity(source);
            source = 0;
        }else{
            storeNode(source);
            storeEdge(edge);
        }
    }
    return source;
}

//SIGNAL
void ModelController::constructNode(int parent_id, NODE_KIND kind, QPointF pos)
{
    //Add undo step
    triggerAction("Constructing Child Node");
    
    auto parent_node = entity_factory->GetNode(parent_id);

    Node* node = 0;
    switch(kind){
        case NODE_KIND::SETTER:{
            node = construct_setter_node(parent_node);
            break;
        }
        case NODE_KIND::QOS_DDS_PROFILE:{
            node = construct_dds_profile_node(parent_node);
            break;
        }
        case NODE_KIND::FOR_CONDITION:{
            node = construct_for_condition_node(parent_node);
            break;
        }
        case NODE_KIND::COMPONENT:{
            node = construct_component_node(parent_node);
            break;
        }
        
        default:
            node = construct_child_node(parent_node, kind);
    }

    if(node){
        //Use position?
        setData_(node, "x", pos.x());
        setData_(node, "y", pos.y());
    }
    emit ActionFinished();
}

void ModelController::constructWorkerProcess(int parent_id, int dst_id, QPointF pos){
    //Add undo step
    triggerAction("Constructing Worker Process");
    
    auto parent_node = entity_factory->GetNode(parent_id);
    auto worker_node = entity_factory->GetNode(dst_id);
    if(parent_node && worker_node && worker_node->getNodeKind() == NODE_KIND::WORKER_PROCESS){
        //clone the worker into the parent
        auto node = cloneNode(worker_node, parent_node);
        if(node){
            //Use position?
            setData_(node, "x", pos.x());
            setData_(node, "y", pos.y());
        }
    }
}

void ModelController::constructEdge(QList<int> src_ids, int dst_id, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    auto valid_dsts = _getConnectableNodes(getNodes(src_ids), edge_kind);
    auto dst = entity_factory->GetNode(dst_id);
    bool success = true;
    if(dst && valid_dsts.contains(dst)){
        triggerAction("Constructing child edge");
        for(auto src_id : src_ids){
            auto src = entity_factory->GetNode(src_id);
            auto edge = construct_edge(edge_kind, src, dst);
            
            if(!edge){
                success = false;
                break;
            }
        }
    }
    emit ActionFinished();
}

void ModelController::constructEdges(QList<int> src_ids, QList<int> dst_ids, EDGE_KIND edge_kind){
    QWriteLocker lock(&lock_);
    
    auto src_nodes = getNodes(src_ids);
    auto dst_nodes = getNodes(dst_ids);
    bool success = true;
    
    //Have to go all to one
    if(src_nodes.count() == 1 || dst_nodes.count() == 1){
        qCritical() << "VALID YO HOLMES";
        for(auto src : src_nodes){
            for(auto dst : dst_nodes){
                qCritical() << src << dst;
                auto edge = construct_edge(edge_kind, src, dst);
                if(!edge){
                    success = false;
                    break;
                }else{
                    qCritical() << edge;
                }
            }
        }
    }
    emit ActionFinished();
}


void ModelController::destructEdges(QList<int> src_ids, int dst_id, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing edges");

    QList<Entity*> edges;
    auto dst = entity_factory->GetNode(dst_id);
    for(auto src_id : src_ids){
        auto src = entity_factory->GetNode(src_id);

        auto edge = src->getEdgeTo(dst, edge_kind);
        if(edge){
            edges << edge;
        }
    }
    destructEntities(edges);
    emit ActionFinished();
}

void ModelController::destructAllEdges(QList<int> src_ids, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing All edges");

    QList<Entity*> edges;

    for(auto src_id : src_ids){
        auto src = entity_factory->GetNode(src_id);
        for(auto edge: src->getEdges(0, edge_kind)){
            if(edge){
                edges << edge;
            }



            
        }
    }
    destructEntities(edges);
    emit ActionFinished();
}



Node* ModelController::construct_setter_node(Node* parent)
{
    if(parent){
        auto node = construct_child_node(parent, NODE_KIND::SETTER);
        if(node){
            auto variable = construct_child_node(node, NODE_KIND::INPUT_PARAMETER);
            auto value = construct_child_node(node, NODE_KIND::VARIADIC_PARAMETER);

            setData_(variable, "icon", "Variable");
            setData_(variable, "icon_prefix", "EntityIcons");
            setData_(variable, "label", "Variable");

            value->setDataValue("icon", "Variable");
            value->setDataValue("icon_prefix", "EntityIcons");

            value->setDataValue("icon", "Variable");
            value->setDataValue("icon_prefix", "EntityIcons");
            value->setDataValue("label", "value");
            return node;
        }
    }
    return 0;
}


 
Node* ModelController::construct_dds_profile_node(Node* parent)
{
    if(parent){
        auto profile = construct_child_node(parent, NODE_KIND::QOS_DDS_PROFILE);
        
        if(profile){
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_DEADLINE);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_DURABILITY);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_GROUPDATA);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_HISTORY);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_LIFESPAN);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_LIVELINESS);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_OWNERSHIP);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_PARTITION);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_PRESENTATION);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_RELIABILITY);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_TOPICDATA);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_USERDATA);
            construct_child_node(profile, NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE);
            return profile;
        }
    }
    return 0;
}




Node* ModelController::construct_for_condition_node(Node* parent)
{
    if(parent){
        triggerAction("Constructing For Condition");


        auto node = construct_child_node(parent, NODE_KIND::FOR_CONDITION);
        if(node){
            auto variable = construct_child_node(node, NODE_KIND::VARIABLE_PARAMETER);
            auto condition = construct_child_node(node, NODE_KIND::INPUT_PARAMETER);
            auto itteration = construct_child_node(node, NODE_KIND::INPUT_PARAMETER);

            variable->setDataValue("type", "Integer");
            variable->setDataValue("label", "i");
            variable->setDataValue("value", 0);
            condition->setDataValue("type", "String");
            condition->setDataValue("label", "Condition");
            condition->setDataValue("icon", "Condition");
            condition->setDataValue("icon_prefix", "EntityIcons");
            itteration->setDataValue("type", "String");
            itteration->setDataValue("label", "Itteration");
            itteration->setDataValue("icon", "reload");
            itteration->setDataValue("icon_prefix", "Icons");
            return node;
        }
    }
    return 0;
}


void ModelController::constructConnectedNode(int id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, QPointF pos)
{
    QWriteLocker lock(&lock_);

    auto parent_node = entity_factory->GetNode(id);
    auto dst_node = entity_factory->GetNode(dst_id);

    if(parent_node && dst_node){
        triggerAction("Constructed Connected Node");

        Node* node = 0;
        if(node_kind == NODE_KIND::WORKER_PROCESS){
            node = cloneNode(dst_node, parent_node);
        }else{
            node = construct_connected_node(parent_node, node_kind, dst_node, edge_kind);
        }

        if(node){
            //Use position?
            setData_(node, "x", pos.x());
            setData_(node, "y", pos.y());
        }

    }
    emit ActionFinished();
}




Node* ModelController::construct_component_node(Node* parent){
    if(parent){
        triggerAction("Constructing Component");

        auto node = construct_child_node(parent, NODE_KIND::COMPONENT);
        if(node){
            auto impl = construct_connected_node(behaviourDefinitions, NODE_KIND::COMPONENT_IMPL, node, EDGE_KIND::DEFINITION);
            return node;
        }
    }
    return 0;
}



Edge* ModelController::construct_edge(EDGE_KIND edge_kind, Node *src, Node *dst, int id, bool notify_view)
{
    Edge* edge = entity_factory->CreateEdge(src, dst, edge_kind, id);

    if(edge && notify_view){
        storeEdge(edge);
    }
    return edge;
}


QList<Node *> ModelController::getNodes(QList<int> ids)
{
    QList<Node* > nodes;
    for(auto id : ids){
        auto node = entity_factory->GetNode(id);
        if(node){
            nodes << node;
        }
    }
    return nodes;
}


void ModelController::triggerAction(QString actionName)
{
    actionCount++;
    currentAction = actionName;
    currentActionID = actionCount;
    emit UndoRedoUpdated();
}

void ModelController::undo()
{   
    if(canUndo()){
        ProgressUpdated_("Undoing");
        setModelAction(MODEL_ACTION::UNDO);
        undoRedo();
        unsetModelAction(MODEL_ACTION::UNDO);
        emit ShowProgress(false);
    }
    emit ActionFinished();
}

void ModelController::redo()
{
    if(canRedo()){
        ProgressUpdated_("Redoing");
        setModelAction(MODEL_ACTION::REDO);
        undoRedo();
        unsetModelAction(MODEL_ACTION::REDO);
        emit ShowProgress(false);
    }
    emit ActionFinished();
}

/**
 * @brief NewController::copy - Attempts to copy a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void ModelController::copy(QList<int> ids)
{
    QReadLocker lock(&lock_);
    auto selection = getOrderedEntities(ids);

    bool success = false;
    if(canCopy(selection)){
        auto value = _copy(selection);

        if(!value.isEmpty()){
            emit SetClipboardData(value);
            success = true;
        }
    }

    emit ActionFinished();
}

void ModelController::remove(QList<int> ids)
{
    QWriteLocker lock(&lock_);
    auto selection = getOrderedEntities(ids);
    bool success = false;
    if(canRemove(selection)){
        triggerAction("Removing Selection");
        success = destructEntities(selection);
    }
    emit ActionFinished();
}

void ModelController::replicate(QList<int> IDs)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(IDs);
    bool success = false;

    if(canReplicate(selection)){
        success = _replicate(selection);
    }

    emit ActionFinished();
}

void ModelController::cut(QList<int> ids)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(ids);
    bool success = false;
    if(canCut(selection)){
        auto data = exportGraphML(selection, true);
        emit SetClipboardData(data);
        emit triggerAction("Cutting Selection");
        success = destructEntities(selection);
    }

    emit ActionFinished();
}

void ModelController::paste(QList<int> ids, QString xml)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(ids);
    bool success = false;

    if(canPaste(selection)){
        auto first = selection.first();
        if(first->isNode()){
            success = _paste((Node*)first, xml);
        }
    }

    emit ActionFinished();
}

bool ModelController::_paste(Node* node, QString xml)
{
    if(node){
        triggerAction("Pasting Selection.");
        //Paste it into the current Selected Node,
        setModelAction(MODEL_ACTION::PASTE);
        bool success = importGraphML(xml, node);
        unsetModelAction(MODEL_ACTION::PASTE);
        return success;
    }
    return false;
}

bool ModelController::_replicate(QList<Entity *> items)
{
    auto xml = exportGraphML(items, true);
    //Paste into the first parent node
    for(auto item : items){
        if(item->isNode()){
            auto parent_node = ((Node*)item)->getParentNode();
            if(parent_node){
                return _paste(parent_node, xml);
            }
        }
    }
    return false;
}


QList<int> ModelController::getConnectableNodeIDs(QList<int> srcs, EDGE_KIND edgeKind)
{
    QReadLocker lock(&lock_);
    auto nodes = _getConnectableNodes(getNodes(srcs), edgeKind);
    
    return getIDs(nodes);
}

QList<int> ModelController::getConstructableConnectableNodes(int parentID, NODE_KIND kind, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);
    
    QList<Node*> nodes;

    auto parent_node = entity_factory->GetNode(parentID);
    auto temp_node = construct_temp_node(parent_node, kind);
    if(temp_node){
        nodes = _getConnectableNodes({temp_node}, edge_kind);
        entity_factory->DestructEntity(temp_node);
    }
    //qCritical() << nodes;
    return getIDs(nodes);
}

QMap<EDGE_DIRECTION, int> ModelController::getConnectableNodeIds2(QList<int> src_ids, EDGE_KIND edge_kind){
    QWriteLocker lock(&lock_);
    
    QMap<EDGE_DIRECTION, int> id_map;
    qCritical() << EntityFactory::getEdgeKindString(edge_kind);
    auto node_map = _getConnectableNodes2(getNodes(src_ids), edge_kind);

    for(auto key : node_map.uniqueKeys()){
        qCritical() << (key == EDGE_DIRECTION::SOURCE ? " SRC2DST: " : " DST2SRC: ");
        for(auto value : node_map.values(key)){
            qCritical() << "\t NODE: " << value;
            id_map.insertMulti(key, value->getID());
        }
    }
    return id_map;
}

QMap<EDGE_DIRECTION, Node*> ModelController::_getConnectableNodes2(QList<Node*> src_nodes, EDGE_KIND edge_kind){
    QMap<EDGE_DIRECTION, Node*> node_map;
    
    bool srcs_require_edge = true;
    
    //Check if they can all accept edges of the kind we care about.
    for(auto src : src_nodes){
        //Check to see if the src Node requires an edge of edge_kind
        if(!src->requiresEdgeKind(edge_kind)){
            srcs_require_edge = false;
            break;
        }
    }

    //Only itterate if we have nodes
    if(srcs_require_edge && src_nodes.size()){
        for(auto id : node_ids_){
            auto dst = entity_factory->GetNode(id);
            if(dst && dst->acceptsEdgeKind(edge_kind)){
                bool src2dst_valid = true;
                bool dst2src_valid = true;

                
                for(auto src : src_nodes){
                    //Only check if for edge adoption if true, otherwise something can't adopt and thus no need to check
                    src2dst_valid = src2dst_valid ? src->canAcceptEdge(edge_kind, dst) : false;
                    dst2src_valid = dst2src_valid ? dst->canAcceptEdge(edge_kind, src) : false;
                }
                
                if(src2dst_valid){
                    node_map.insertMulti(EDGE_DIRECTION::TARGET, dst);
                }
                if(dst2src_valid){
                    node_map.insertMulti(EDGE_DIRECTION::SOURCE, dst);
                }
            }
        }
    }
    return node_map;
}

QList<Node *> ModelController::_getConnectableNodes(QList<Node *> src_nodes, EDGE_KIND edge_kind){
    QList<Node*> valid_nodes;

    bool srcs_require_edge = true;
    bool try_backwards = edge_kind == EDGE_KIND::ASSEMBLY;

    //Check if they can all accept edges of the kind we care about.
    for(auto src : src_nodes){
        //Check to see if the src Node requires an edge of edge_kind
        if(!src->requiresEdgeKind(edge_kind)){
            srcs_require_edge = false;
            break;
        }
    }
    
    if(!src_nodes.empty() && srcs_require_edge){
        for(auto id : node_ids_){
            auto dst = entity_factory->GetNode(id);
            if(dst && dst->acceptsEdgeKind(edge_kind)){
                bool all_valid = true;
                for(auto src : src_nodes){
                    auto src2dst = src->canAcceptEdge(edge_kind, dst);
                    auto dst2src = try_backwards && dst->canAcceptEdge(edge_kind, src);
                    auto valid = src2dst || dst2src;
                    if(!valid){
                        all_valid = false;
                        break;
                    }
                }
                if(all_valid){
                    valid_nodes.append(dst);
                }
            }
        }
    }
    return valid_nodes;
}

QList<int> ModelController::getWorkerFunctions()
{
    QReadLocker lock(&lock_);
    auto worker_processes = workerDefinitions->getChildrenOfKind(NODE_KIND::WORKER_DEFINITIONS);
    return getIDs(worker_processes);
}

QList<Entity*> ModelController::getOrderedEntities(QList<int> ids){
    return getOrderedEntities(getUnorderedEntities(ids));
}

QList<Entity*> ModelController::getOrderedEntities(QList<Entity*> entities){
    //Sort the node (Deepest Nodes later, edges last)
    std::sort(entities.begin(), entities.end(), [](const Entity* a, const Entity* b){
        auto is_a_node = a->isNode();
        auto is_b_node = b->isNode();

        if(is_a_node && is_b_node){
            auto a_node = (const Node*)a;
            auto b_node = (const Node*)b;
            //Proper depth
            auto a_depth = a_node->getDepth();
            auto b_depth = b_node->getDepth();
            return a_depth < b_depth;
        }else{
            return is_a_node;
        }
        });

    //return entities;
    QList<Entity*> top_entities;
    
    //Foreach node
    for(auto node : entities){
        bool got_parent = false;
        //Check the node if we have a parent for it.
        for(auto top: top_entities){
            if(top->isNode()){
                auto parent_node = (Node*) top;
                if(parent_node->isAncestorOf(node)){
                    got_parent = true;
                    break;
                }
            }
        }
        if(!got_parent){
            top_entities << node;
        }
    }
    return top_entities;
}

QList<QVariant> ModelController::getValidKeyValues(int id, QString key_name)
{
    QReadLocker lock(&lock_);
    QList<QVariant> valid_values;
    
    Key* key = entity_factory->GetKey(key_name);
    if(key){
        auto node = entity_factory->GetNode(id);
        if(node){
            valid_values = key->getValidValues(node->getNodeKind());
        }
    }
    return valid_values;
}

int ModelController::getSharedParent(int id, int id2){
    QReadLocker lock(&lock_);

    auto node_1 = entity_factory->GetNode(id);
    auto node_2 = entity_factory->GetNode(id2);
    if(node_1 && node_2){
        auto parent = node_1->getCommonAncestor(node_2);
        if(parent){
            return parent->getID();
        }
    }
    return -1;
}

bool ModelController::isNodeAncestor(int ID, int ID2){
    QReadLocker lock(&lock_);
    bool is_ancestor = false;
    auto node_1 = entity_factory->GetNode(ID);
    auto node_2 = entity_factory->GetNode(ID2);
    if(node_1 && node_2){
        is_ancestor = node_1->isAncestorOf(node_2);
    }
    return is_ancestor;
}

int ModelController::getNodeParentID(int ID){
    QReadLocker lock(&lock_);
    int parent_id = -1;
    auto node = entity_factory->GetNode(ID);
    
    if(node){
        auto parent = node->getParentNode();
        if(parent){
            parent_id = parent->getID();
        }
    }
    return parent_id;
}

VIEW_ASPECT ModelController::getNodeViewAspect(int ID){
    QReadLocker lock(&lock_);
    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;
    auto node = entity_factory->GetNode(ID);
    
    if(node){
        aspect = node->getViewAspect();
    }
    return aspect;
}

QStringList ModelController::getEntityKeys(int ID){
    QReadLocker lock(&lock_);
    QStringList keys;

    auto entity = entity_factory->GetEntity(ID);
    if(entity){
        keys = entity->getKeyNames();
    }
    return keys;
}

QStringList ModelController::getProtectedEntityKeys(int ID){
    QReadLocker lock(&lock_);
    QStringList keys;

    auto entity = entity_factory->GetEntity(ID);
    if(entity){
        for(auto data: entity->getData()){
            if(data->isProtected()){
                keys << data->getKeyName();
            }
        }
    }
    return keys;
}



QVariant ModelController::getEntityDataValue(int ID, QString key_name){
    QReadLocker lock(&lock_);
    QVariant value;

    auto entity = entity_factory->GetEntity(ID);
    if(entity){
        value = entity->getDataValue(key_name);
    }
    return value;
}


bool ModelController::isNodeOfType(int ID, NODE_TYPE type){
    QReadLocker lock(&lock_);
    bool is_type = false;
    auto node = entity_factory->GetNode(ID);
    if(node){
        is_type = node->isNodeOfType(type);
    }
    return is_type;
}

QList<int> ModelController::getInstances(int ID)
{
    QReadLocker lock(&lock_);
    QList<int> instances;

    for(auto node : getInstances(entity_factory->GetNode(ID))){
        instances.push_back(node->getID());
    }
    return instances;
}

int ModelController::getAggregate(int ID)
{
    int aggrID = -1;
    Node* node = entity_factory->GetNode(ID);
    if(node){
        if(node->isNodeOfType(NODE_TYPE::EVENTPORT)){
            EventPort* eventPort = (EventPort*)node;
            if(eventPort && eventPort->getAggregate()){
                aggrID = eventPort->getAggregate()->getID();
            }
        }
    }
    return aggrID;

}

//TODO REFACTORED
int ModelController::getDeployedHardwareID(int ID)
{
    int hw_id = -1;
    Node* node = entity_factory->GetNode(ID);
    if(node){
        for(auto edge : node->getEdges(0, EDGE_KIND::DEPLOYMENT)){
            hw_id = edge->getDestinationID();
        }
    }
    return hw_id;
}

void ModelController::setProjectSaved(QString path)
{
    setProjectModified(false);
    if(path != ""){
        //Update the file save path.
        setProjectPath(path);
    }
}



void ModelController::storeEntity(Entity* item)
{
    if(item){
        auto id = item->getID();
        auto kind = item->getGraphMLKind();

        //Connect things!
        connect(item, &Entity::dataChanged, this, &ModelController::DataChanged, Qt::UniqueConnection);
        connect(item, &Entity::dataRemoved, this, &ModelController::DataRemoved, Qt::UniqueConnection);
    
        switch(kind){
        case GRAPHML_KIND::NODE:{
            node_ids_.insert(id);
            auto node = (Node*) item;
            emit NodeConstructed(node->getParentNodeID(), id, node->getNodeKind());
            break;
        }
        case GRAPHML_KIND::EDGE:{
            edge_ids_.insert(id);
            auto edge = (Edge*) item;
            emit EdgeConstructed(id, edge->getEdgeKind(), edge->getSourceID(), edge->getDestinationID());
            break;
        }
        default:
            break;
        }
    }
}


void ModelController::removeEntity(Entity* entity)
{
    if(entity){
        auto id = entity->getID();
        auto kind = entity->getGraphMLKind();
        emit EntityDestructed(id, kind);
        node_ids_.remove(id);
        edge_ids_.remove(id);
        entity_factory->DestructEntity(entity);
    }
}


bool ModelController::attachChildNode(Node *parentNode, Node *node, bool notify_view)
{
    if(parentNode->addChild(node)){
        if(notify_view){
            storeNode(node);
        }

        if(isUserAction()){
            for(auto dependant : parentNode->getDependants()){
                //Setup dependant relationship only for user functions
                constructDependantRelative(dependant, node);
            }
        }
        return true;
    }
    return false;
}

QList<int> ModelController::getIDs(QList<Entity *> items)
{
    QList<int> ids;
    for(auto item : items){
        ids << item->getID();
    }
    return ids;
}

QList<int> ModelController::getIDs(QList<Node *> nodes)
{
    QList<int> ids;
    for(auto item : nodes){
        ids << item->getID();
    }
    return ids;
}


QList<Entity *> ModelController::getUnorderedEntities(QList<int> ids){
    QList<Entity*> entities;
    for(auto id : ids){
        auto entity = entity_factory->GetEntity(id);
        if(entity){
            entities << entity;
        }
    }
    return entities;
}

Node *ModelController::cloneNode(Node *original, Node *parent)
{
    if(original && parent){
        auto node = construct_child_node(parent, original->getNodeKind());
        
        if(node){
            //Get the data
            for(auto data : original->getData()){
                setData_(node, data->getKeyName(), data->getValue());
            }

            for(auto child : original->getChildren(0)){
                cloneNode(child, node);
            }
        }
        return node;
    }
    return 0;
}


int ModelController::constructDependantRelative(Node *parent, Node *definition)
{
    int nodes_matched = 0;
    
    auto dependant_kind = NODE_KIND::NONE;

    if(parent->isInstance()){
        dependant_kind = definition->getInstanceKind();
    }else{
        dependant_kind = definition->getImplKind();
    }

    //For each child in parent, check to see if any Nodes match Label/Type
    for(auto child : parent->getChildrenOfKind(dependant_kind, 0)){
        if(!child->getDefinition()){
            auto labels_match = child->compareData(definition, "label");
            auto types_match = child->compareData(definition, "type");

            //If the labels and types match, we can construct an edge between them
            if(types_match && labels_match){
                construct_edge(EDGE_KIND::DEFINITION, child, definition);
            }
        }
        if(child->getDefinition() == definition){
            nodes_matched ++;    
        }
    }

    if(!nodes_matched){
        //If we didn't find a match, we must create an Instance.
        auto node = construct_connected_node(parent, dependant_kind, definition, EDGE_KIND::DEFINITION);
        if(node){
            nodes_matched ++;
        }
    }
    return nodes_matched;
}


void ModelController::destructEdge_(Edge *edge){
    if(edge){
        auto id = edge->getID();
        auto src = edge->getSource();
        auto dst = edge->getDestination();
        if(src && dst){
            //Teardown specific edge classes.
            auto edge_kind = edge->getEdgeKind();

            switch(edge_kind){
            case EDGE_KIND::DEFINITION:{
                setupDefinitionRelationship(src, dst, false);
                break;
            }
            case EDGE_KIND::AGGREGATE:{
                setupAggregateRelationship(src, dst, false);
                break;
            }
            case EDGE_KIND::DATA:{
                setupDataRelationship(src, dst, false);                
                break;
            }
            default:
                break;
            }

            //Remove it from the hash of GraphML
            removeEntity(edge);
        }
    }
}

bool ModelController::destructEntity(int ID){
    return destructEntity(entity_factory->GetEntity(ID));
}
bool ModelController::destructEntity(Entity* item){
    return destructEntities(QList<Entity*>{item});
}

bool ModelController::destructEntities(QList<Entity*> entities)
{
    QSet<Entity*> nodes;
    QSet<Entity*> edges;

    for(auto entity : entities){
        if(entity){
            if(entity->isNode()){
                auto node = (Node*) entity;
                nodes.insert(node);
                for(auto dependant : node->getNestedDependants()){
                    for(auto child_node : dependant->getChildren()){
                        nodes.insert(child_node);
                    }
                    nodes.insert(dependant);
                }
            }else if(entity->isEdge()){
                auto edge = (Edge*) entity;
                edges.insert(edge);
            }
        }
    }

  
    //Get sorted orders
    auto sorted_nodes = getOrderedEntities(nodes.toList());

    //Get all the edges
    for(auto n : sorted_nodes){
        auto node = (Node*) n;
        for(auto edge : node->getAllEdges()){
            edges.insert(edge);
        }
    }

    if(!edges.empty()){
        //Create an undo state which groups all edges together
        auto action = getNewAction(GRAPHML_KIND::EDGE);
        action.Action.type = ACTION_TYPE::DESTRUCTED;
        action.xml = exportGraphML(edges.toList(), true);
        addActionToStack(action);

        //Destruct all the edges
        for(auto e : edges){
            destructEdge_((Edge*)e);
        }
    }

    for(auto entity : sorted_nodes){
        auto node = (Node*) entity;

        //Create an undo state for each top level item
        auto action = getNewAction(GRAPHML_KIND::NODE);
        action.entity_id = entity->getID();
        action.parent_id = node->getParentNodeID();
        action.Action.type = ACTION_TYPE::DESTRUCTED;
        action.xml = exportGraphML(entity);
        addActionToStack(action);
        destructNode_(node);
    }
    return true;
}


void ModelController::destructNode_(Node* node){
    if(node){
        //Remove children
        auto children = node->getChildren();
        while(!children.isEmpty()){
            //delete from the end.
            auto child = children.takeLast();
            removeEntity(child);

        }
        removeEntity(node);
    }
}


bool ModelController::reverseAction(HistoryAction action)
{   
    switch(action.Action.type){
    case ACTION_TYPE::CONSTRUCTED:{
        switch(action.Action.kind){
        case GRAPHML_KIND::NODE:
        case GRAPHML_KIND::EDGE:{
            return destructEntity(action.entity_id);
        }
        case GRAPHML_KIND::DATA:{
            return destructData_(entity_factory->GetEntity(action.entity_id), action.Data.key_name);
        }
        default:
            break;
        }
        break;
    }
    case ACTION_TYPE::DESTRUCTED:{
        switch(action.Action.kind){
        case GRAPHML_KIND::NODE:
        case GRAPHML_KIND::EDGE:{
            auto parent_node = entity_factory->GetNode(action.parent_id);
            //Don't pass a MODEL_ACTION
            return importGraphML(action.xml, parent_node);
        }
        case GRAPHML_KIND::DATA:{
            auto entity = entity_factory->GetEntity(action.entity_id);
            return setData_(entity, action.Data.key_name, action.Data.old_value);
        }
        default:
            break;
        }
        break;
    }
    case ACTION_TYPE::MODIFIED:{
        switch(action.Action.kind){
        case GRAPHML_KIND::DATA:{
            auto entity = entity_factory->GetEntity(action.entity_id);
            return setData_(entity, action.Data.key_name, action.Data.old_value);
        }
        default:
            break;
        }
        break;
    }
    }
    return false;
}


void ModelController::addActionToStack(HistoryAction action, bool useAction)
{
    //Change made, make dirty the project
    setProjectModified(true);

    if(useAction){
        if(isModelAction(MODEL_ACTION::UNDO)){
            redo_stack.push(action);
        }else{
            undo_stack.push(action);
        }
        emit UndoRedoUpdated();
    }
}

bool ModelController::undoRedo()
{
    QStack<HistoryAction> action_stack;

    //replace the stacks
    if(isModelAction(MODEL_ACTION::UNDO)){
        undo_stack.swap(action_stack);
    }else if(isModelAction(MODEL_ACTION::REDO)){
        redo_stack.swap(action_stack);
    }

    auto action_id = action_stack.top().Action.id;
    auto action_name = action_stack.top().Action.name;

    triggerAction(action_name);

    QList<HistoryAction> reverse_actions;
    
    while(!action_stack.empty()){
        auto action = action_stack.top();
        if(action.Action.id == action_id){
            action_stack.pop();
            //If the action has the same id as the top, it's part of the same state.
            reverse_actions.push_back(action);
        }else{
            break;
        }
    }
    
    double action_count = reverse_actions.size() / 100.0;
    int actions_reversed = 0;
    
    bool success = true;
    for(auto action : reverse_actions){
        if(reverseAction(action)){
            actions_reversed ++;
            ProgressChanged_(++actions_reversed / action_count);
        }else{
            success = false;
            qCritical() << "FAILED TO UNDO";

            qCritical() << entity_factory->GetEntity(action.entity_id);
            qCritical() << "ENTITY ID: " << action.entity_id;
            qCritical() << "GRAPHML_KIND:" << ((uint)action.Action.kind);
            qCritical() << "TYPE:" << ((uint)action.Action.type);
            qCritical() << "action.Data.key_name:" << action.Data.key_name;
            qCritical() << "action.Data.old_value:" << action.Data.old_value;
            qCritical() << "action.Data.new_value:" << action.Data.new_value;
        }
    }

    //replace the stacks
    if(isModelAction(MODEL_ACTION::UNDO)){
        undo_stack.swap(action_stack);
    }else if(isModelAction(MODEL_ACTION::REDO)){
        redo_stack.swap(action_stack);
    }

    emit UndoRedoUpdated();
    return success;
}

bool ModelController::canDeleteNode(Node *node)
{
    if(node){
        if(protected_nodes.contains(node)){
            return false;
        }
        auto node_kind = node->getNodeKind();

        switch(node_kind){
            case NODE_KIND::INPUT_PARAMETER:
            case NODE_KIND::RETURN_PARAMETER:
                return false;
            default:
                break;
        }

        if(node->getDefinition()){
            switch(node_kind){
            case NODE_KIND::OUTEVENTPORT_IMPL:
            case NODE_KIND::COMPONENT_INSTANCE:
                //CompoentInstances/Outeventportimpls can be destroyed at any time
                break;
            case NODE_KIND::AGGREGATE_INSTANCE:{
                auto parent_node = node->getParentNode();
                auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;

                switch(parent_node_kind){
                    case NODE_KIND::AGGREGATE:
                    case NODE_KIND::VECTOR:
                        break;
                    default:
                        return false;
                }
                break;
            }
            default:
                //Can't remove instances
                return false;
            }
        }
    }

    return true;
}





void ModelController::clearHistory()
{
    currentActionID = 0;
    actionCount = 0;
    currentAction = "";
    undo_stack.clear();
    redo_stack.clear();
    emit UndoRedoUpdated();
}

void ModelController::ModelNameChanged()
{
    if(model){
        emit ProjectNameChanged(model->getDataValue("label").toString());
    }
}

EDGE_KIND ModelController::getValidEdgeClass(Node *src, Node *dst)
{
    foreach(EDGE_KIND edgeClass, entity_factory->getEdgeKinds()){
        if(src->canAcceptEdge(edgeClass, dst)){
            return edgeClass;
        }
    }
    return EDGE_KIND::NONE;
}

QList<EDGE_KIND> ModelController::getPotentialEdgeClasses(Node *src, Node *dst)
{
    QList<EDGE_KIND> edgeKinds;

    foreach(EDGE_KIND edgeClass, GetEdgeOrderIndexes()){
        if(src->acceptsEdgeKind(edgeClass) && dst->acceptsEdgeKind(edgeClass) && src->requiresEdgeKind(edgeClass)){
            edgeKinds << edgeClass;
        }
    }
    return edgeKinds;
}

QString ModelController::_copy(QList<Entity *> selection)
{
    return exportGraphML(selection, false);
}


void ModelController::storeNode(Node *node)
{
    if(node){
        //Construct an ActionItem to reverse Node Construction.
        auto action = getNewAction(GRAPHML_KIND::NODE);
        action.Action.type = ACTION_TYPE::CONSTRUCTED;
        action.entity_id = node->getID();
        action.parent_id = node->getParentNodeID();
        //Add Action to the Undo/Redo Stack.
        addActionToStack(action);

        //Add the node to the list of nodes constructed.
        storeEntity(node);
    }
}

void ModelController::setupModel()
{
    //Construct the top level parents.
    model = construct_node(0, NODE_KIND::MODEL);
    workerDefinitions = construct_node(0, NODE_KIND::WORKER_DEFINITIONS);

    Data* labelData = model->getData("label");
    if(labelData){
        connect(labelData, &Data::dataChanged, this, &ModelController::ModelNameChanged);
         //Update the view with the correct Model Label.
        ModelNameChanged();
    }

    //Construct the aspects
    interfaceDefinitions = construct_child_node(model, NODE_KIND::INTERFACE_DEFINITIONS, false);
    behaviourDefinitions = construct_child_node(model, NODE_KIND::BEHAVIOUR_DEFINITIONS, false);
    deploymentDefinitions = construct_child_node(model, NODE_KIND::DEPLOYMENT_DEFINITIONS, false);
    assemblyDefinitions = construct_child_node(deploymentDefinitions, NODE_KIND::ASSEMBLY_DEFINITIONS, false);
    hardwareDefinitions = construct_child_node(deploymentDefinitions, NODE_KIND::HARDWARE_DEFINITIONS, false);

    //Construct the localhost node
    auto localhostNode = construct_child_node(hardwareDefinitions, NODE_KIND::HARDWARE_NODE, false);
    localhostNode->setDataValue("label", "localhost");
    localhostNode->setDataValue("ip_address", "127.0.0.1");
    localhostNode->setDataValue("os", QSysInfo::productType());
    localhostNode->setDataValue("os_version", QSysInfo::productVersion());
    localhostNode->setDataValue("architecture", QSysInfo::currentCpuArchitecture());
    localhostNode->setDataValue("uuid", "localhost");

    //Protect the nodes
    protected_nodes << model;
    protected_nodes << workerDefinitions;
    protected_nodes << interfaceDefinitions;
    protected_nodes << behaviourDefinitions;
    protected_nodes << deploymentDefinitions;
    protected_nodes << assemblyDefinitions;
    protected_nodes << hardwareDefinitions;
    protected_nodes << localhostNode;

    //construct the GUI
    for(auto node : protected_nodes){
        storeNode(node);
    }
}



void ModelController::bindData(Node *defn, Node *child)
{
    if(!defn || !child){
        return;
    }

    auto defn_kind = defn->getNodeKind();
    auto parent_kind = NODE_KIND::NONE;
    if(child->getParentNode()){
        parent_kind = child->getParentNode()->getNodeKind();
    }
    auto child_kind = child->getNodeKind();

    auto defn_type = defn->getData("type");
    auto defn_label = defn->getData("label");
    auto defn_key = defn->getData("key");
    auto defn_index = defn->getData("index");

    auto child_type = child->getData("type");
    auto child_label = child->getData("label");
    auto child_key = child->getData("key");
    auto child_index = child->getData("index");

    //We can't bind to any of these if they aren't set
    bool bind_types = child_type;
    bool bind_labels = child_label && defn_label;
    bool bind_keys = child_key && defn_key;
    bool bind_index = child_index && defn_index;

    //The only time we should bind the index is when we are contained in another instance
    if(!child->getParentNode()->isInstance()){
        bind_index = false;
    }
    
    if(child->isInstance() || child->isImpl()){
        if(child_kind == NODE_KIND::COMPONENT_INSTANCE || child_kind == NODE_KIND::BLACKBOX_INSTANCE){
            //Allow ComponentInstance and BlackBoxInstance to have unique labels
            bind_labels = false;
        }else if(child_kind == NODE_KIND::AGGREGATE_INSTANCE || child_kind == NODE_KIND::VECTOR_INSTANCE){
            if(parent_kind == NODE_KIND::AGGREGATE){
                //Allow Aggregates to contain Aggregate Instances with unique labels
                bind_labels = false;
            }
        }
    }else if(child->isDefinition()){
        bind_labels = false;
    }

    //Bind Type to either Type or Label
    
    if(bind_types){
        if(defn_type){
            defn_type->bindData(child_type);
        }else if(defn_label){
            defn_label->bindData(child_type);
        }
    }

    //Bind label
    if(bind_labels){
        defn_label->bindData(child_label);
    }

    //Bind Keys
    if(bind_keys){
        defn_key->bindData(child_key);
    }

    //Bind Index
    if(bind_index){
        defn_index->bindData(child_index);
    }
}

void ModelController::unbindData(Node *definition, Node *instance)
{
    foreach(Data* attachedData, definition->getData()){
        Data* newData = 0;
        newData = instance->getData(attachedData->getKey());
        if(newData){
            attachedData->unbindData(newData);
        }
    }
}

/**
 * @brief NewController::setupDefinitionRelationship
 * Attempts to construct/set the provided node as an Instance of the definition provided.
 * Will Adopt Instances of all Definitions contained by Definition provided. Binds relevant Data together.
 * @param definition - The Node which is the Definition of the relationship.
 * @param aggregate - The Node which is to be set as the Instance.
 * @param instance - Is this an Instance or Implementation Relationship.
 * @return true if Definition Relation was setup correctly.
 */
bool ModelController::setupDefinitionRelationship(Node *src, Node *dst, bool setup)
{
    if(src && dst){
        auto node_kind = src->getNodeKind();
        auto construct_dependant = setup && isUserAction() && (node_kind != NODE_KIND::INEVENTPORT_INSTANCE && node_kind != NODE_KIND::OUTEVENTPORT_INSTANCE) ;
        
        if(construct_dependant){
            for(auto child : dst->getChildren(0)){
                if(child->isNodeOfType(NODE_TYPE::DEFINITION)){
                    if(!constructDependantRelative(src, child)){
                        qCritical() << "setupDefinitionRelationship(): Couldn't create a Definition Relative for: " << child->toString() << " In: " << src->toString();
                        return false;
                    }
                }
            }
        }

        //Check for an edge between the EventPort and the Aggregate
        if(src->gotEdgeTo(dst, EDGE_KIND::DEFINITION)){
            if(setup){
                //Bind the un-protected Data attached to the Definition to the Instance.
                bindData(dst, src);

                if(src->isInstance()){
                    dst->addInstance(src);
                }else{
                    dst->addImplementation(src);
                }
            }else{
                //Bind the un-protected Data attached to the Definition to the Instance.
                unbindData(dst, src);

                if(src->isInstance()){
                    dst->removeInstance(src);
                }else{
                    dst->removeImplementation(src);
                }
            }
            return true;
        }
    }
    return false;
}


/**
 * @brief NewController::setupAggregateRelationship
 * Attempts to find/construct an AggregateInstance of the provided Aggregate inside the EventPort.
 * @param eventPort - The EventPort which the Aggregate is getting set to.
 * @param aggregate - the Aggregate to set.
 * @return true if Aggregate Relation was setup correctly.
 */
bool ModelController::setupAggregateRelationship(Node *src, Node *dst, bool setup)
{
    if(src && dst && src->isNodeOfType(NODE_TYPE::EVENTPORT) && dst->getNodeKind() == NODE_KIND::AGGREGATE){
        auto eventport = (EventPort*) src;
        auto aggregate = (Aggregate*) dst;

        bool construct_instance = setup && isUserAction() && (eventport->getNodeKind() == NODE_KIND::INEVENTPORT || eventport->getNodeKind() == NODE_KIND::OUTEVENTPORT);
        //Only auto construct if we are processing a user action.
        if(construct_instance){
            Node* aggregate_instance = 0;
            
            //Check for a child of kind AggregateInstance
            for(auto child : eventport->getChildrenOfKind(NODE_KIND::AGGREGATE_INSTANCE, 0)){
                aggregate_instance = child;
                break;
            }

            if(!aggregate_instance){
                //Construct a connected AggregateInstance inside the eventport, connected to the aggregate (The instance section will)
                aggregate_instance = construct_connected_node(eventport, NODE_KIND::AGGREGATE_INSTANCE, aggregate, EDGE_KIND::DEFINITION);
            }
            
            //If we have an aggregate instance
            if(aggregate_instance){
                if(!aggregate_instance->gotEdgeTo(aggregate, EDGE_KIND::DEFINITION)){
                    //Try and construct an edge between the AggregateInstance and it's Instance
                    construct_edge(EDGE_KIND::DEFINITION, aggregate_instance, aggregate);
                }
            }
        }

        //Check for an edge between the EventPort and the Aggregate
        if(eventport->gotEdgeTo(aggregate, EDGE_KIND::AGGREGATE)){
            if(setup){
                //Edge Created Set Aggregate relation.
                eventport->setAggregate(aggregate);
                //Bind type to type
                bindData_(aggregate, "type", eventport, "type");
            }else{
                //Unset the Aggregate
                eventport->unsetAggregate();
                //unbind type to type
                unbindData_(aggregate, "type", eventport, "type");
            }
            return true;
        }
    }
    return false;
}

//Data binds the value of src->getData(src_key) into dst->getData(dst_key)
bool ModelController::bindData_(Node* src, QString src_key, Node* dst, QString dst_key){
    if(src && dst){
        auto src_data = src->getData(src_key);
        auto dst_data = dst->getData(dst_key);
        if(src_data && dst_data && !dst_data->getParentData()){
            src_data->bindData(dst_data);
            return true;
        }
    }
    return false;
}

//Data binds the value of src->getData(src_key) into dst->getData(dst_key)
bool ModelController::unbindData_(Node* src, QString src_key, Node* dst, QString dst_key){
    if(src && dst){
        auto src_data = src->getData(src_key);
        auto dst_data = dst->getData(dst_key);
        if(src_data && dst_data && dst_data->getParentData() == src_data){
            src_data->unbindData(dst_data);
            return true;
        }

    }
    return false;
}

bool ModelController::setupDataRelationship(Node* src, Node* dst, bool setup)
{
   if(src && dst && src->isNodeOfType(NODE_TYPE::DATA) && dst->isNodeOfType(NODE_TYPE::DATA)){
        auto src_parent = src->getParentNode();
        auto dst_parent = dst->getParentNode();

        //Bind the special vector linking
        if(dst_parent->getNodeKind() == NODE_KIND::WORKER_PROCESS){
            auto worker_name = dst_parent->getDataValue("worker").toString();
            auto parameter_label = dst->getDataValue("label").toString();

            //Check bindings
            if(worker_name == "Vector_Operations" && parameter_label.contains("Vector")){
                //Get the child type of the Vector
                Node* vector = src;
                Node* vector_child = src->getFirstChild();

                //Get the siblings of the parameter
                for(auto param : dst_parent->getChildren(0)){
                    if(param->isNodeOfType(NODE_TYPE::PARAMETER)){
                        auto param_label = param->getDataValue("label").toString();
                        Node* bind_src = 0;

                        if(param_label.contains("Value")){
                            //Bind the child type to the param
                            bind_src = vector_child;
                        }else if(param_label.contains("Vector")){
                            //Bind the vector type to the param
                            bind_src = vector;
                        }

                        if(setup){
                            bindData_(bind_src, "type", param, "type");
                        }else{
                            unbindData_(bind_src, "type", param, "type");
                        }
                    }
                }
            }
        }

        QString src_key = "type";
        auto bind_src = src;
        
        //Data bind to the Variable, instead of the Member
        if(src_parent && src_parent->getNodeKind() == NODE_KIND::VARIABLE){
            bind_src = src_parent;
        }

        //If what we are binding to is a Variable/AttributeImpl, bind to the label
        if(bind_src->getNodeKind() == NODE_KIND::VARIABLE || bind_src->getNodeKind() == NODE_KIND::ATTRIBUTE_IMPL){
            src_key = "label";
        }

        if(setup){
            bindData_(bind_src, src_key, dst, "value");
        }else{
            unbindData_(bind_src, src_key, dst, "value");
        }
        return true;
   }
   return false;
}


bool ModelController::isGraphMLValid(QString inputGraphML)
{
    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xmlErrorChecking(inputGraphML);

    //Check for Errors
    while(!xmlErrorChecking.atEnd()){
        xmlErrorChecking.readNext();
        if(xmlErrorChecking.hasError()){
            qCritical() << "isGraphMLValid(): Parsing Error! Line #" << xmlErrorChecking.lineNumber();
            qCritical() << "\t" << xmlErrorChecking.errorString();
            return false;
        }
    }
    return true;
}

void ModelController::storeEdge(Edge *edge)
{
    if(edge){
        //Construct an ActionItem to reverse an Edge Construction.
        auto action = getNewAction(GRAPHML_KIND::EDGE);
        action.Action.type = ACTION_TYPE::CONSTRUCTED;
        action.Action.kind = edge->getGraphMLKind();
        action.entity_id = edge->getID();
        //Add Action to the Undo/Redo Stack
        addActionToStack(action);

        auto src = edge->getSource();
        auto dst = edge->getDestination();

        //Do Special GUI related things
        switch(edge->getEdgeKind()){
        case EDGE_KIND::DEFINITION:{
            setupDefinitionRelationship(src, dst, true);
            break;
        }
        case EDGE_KIND::AGGREGATE:{
            setupAggregateRelationship(src, dst, true);
            break;
        }
        case EDGE_KIND::DATA:{
            setupDataRelationship(src, dst, true);
            break;
        }
        default:
            break;
        }

        storeEntity(edge);
    }
}

QString ModelController::getProjectAsGraphML()
{
    lock_.lockForRead();
    QString data = exportGraphML(model);
    lock_.unlock();
    return data;
}


QList<EDGE_KIND> ModelController::getValidEdgeKindsForSelection(QList<int> IDs)
{
    lock_.lockForRead();

    QList<Entity*> entities = getOrderedEntities(IDs);
    QList<EDGE_KIND> edgeKinds;

    if(!entities.isEmpty()){
        edgeKinds = entity_factory->getEdgeKinds();
    }

    foreach(Entity* entity, entities){
        if(entity->isNode()){
            Node* node = (Node*) entity;
            foreach(EDGE_KIND edgeKind, edgeKinds){
                if(!node->requiresEdgeKind(edgeKind)){
                    edgeKinds.removeAll(edgeKind);
                }
            }
        }else{
            //Is edge, no valid edges.
            edgeKinds.clear();
        }
        if(edgeKinds.isEmpty()){
            break;
        }
    }
    lock_.unlock();
    return edgeKinds;
}

QList<EDGE_KIND> ModelController::getExistingEdgeKindsForSelection(QList<int> IDs)
{
    lock_.lockForRead();

    QList<Entity*> entities = getOrderedEntities(IDs);
    QList<EDGE_KIND> edgeKinds;

    foreach(Entity* entity, entities){
        if(entity->isNode()){
            Node* node = (Node*) entity;
            foreach(Edge* edge, node->getAllEdges()){
                if(!edgeKinds.contains(edge->getEdgeKind())){
                    edgeKinds.append(edge->getEdgeKind());
                }
            }
        }
    }
    lock_.unlock();
    return edgeKinds;

}

QSet<NODE_KIND> ModelController::getGUINodeKinds(){
    auto node_set = QSet<NODE_KIND>::fromList(entity_factory->getNodeKinds());
    node_set.remove(NODE_KIND::IDL);
    node_set.remove(NODE_KIND::WORKLOAD);
    node_set.remove(NODE_KIND::PROCESS);
    return node_set;
}

QList<NODE_KIND> ModelController::getAdoptableNodeKinds(int ID)
{
    QReadLocker lock(&lock_);
    QList<NODE_KIND> kinds;

    Node* parent = entity_factory->GetNode(ID);

    //Ignore all children for read only kind.
    if(parent && !parent->isReadOnly()){
        bool can_adopt = !parent->isInstance();
        
        if(can_adopt){
            for(auto node_kind: getGUINodeKinds()){
                auto temp_node = entity_factory->CreateTempNode(node_kind);
                if(temp_node){
                    if(parent->canAdoptChild(temp_node)){
                        kinds << node_kind;
                    }
                    entity_factory->DestructEntity(temp_node);
                }
            }
        }
    }

    return kinds;
}

QStringList ModelController::getVisualKeys()
{
    QStringList visualKeys;
    visualKeys << "x";
    visualKeys << "y";
    visualKeys << "width";
    visualKeys << "height";
    visualKeys << "isExpanded";
    visualKeys << "readOnly";
    return visualKeys;
}



/**
 * @brief NewController::setData Sets the Value of the Data of an Entity.
 * @param parentID - The ID of the Entity
 * @param keyName - The name of the Key
 * @param dataValue - The new value of the Data.
 */
void ModelController::setData(int parentID, QString keyName, QVariant dataValue)
{
    Entity* graphML = entity_factory->GetEntity(parentID);
    if(graphML){
        setData_(graphML, keyName, dataValue, true);
    }
}

void ModelController::removeData(int id, QString key_name)
{
    destructData_(entity_factory->GetEntity(id), key_name);
}



/**
 * @brief NewController::importProjects
 * @param xmlDataList
 */
void ModelController::importProjects(QStringList xml_list)
{
    QWriteLocker lock(&lock_);
    bool success = true;
    ProgressUpdated_("Importing Projects");
    if(xml_list.length() > 0){
        triggerAction("Importing GraphML Projects.");
        setModelAction(MODEL_ACTION::IMPORT);
        for(auto xml : xml_list){
            auto result = importGraphML(xml, model);
            if(!result){
                emit Notification(MODEL_SEVERITY::ERROR, "Cannot import Project.");
                success = false;
            }
        }
        unsetModelAction(MODEL_ACTION::IMPORT);
    }
    emit ShowProgress(false);
    emit ActionFinished();
}

bool ModelController::isDataVisual(Data* data){
    if(data){
        return isKeyNameVisual(data->getKeyName());
    }else{
        return false;
    }
}


void ModelController::ProgressChanged_(int progress){
    if(progress != last_progress){
        last_progress = progress;
        emit ProgressChanged(progress);
    }
}

void ModelController::ProgressUpdated_(QString description){
    if(description != last_description){
        last_description = description;
        emit ShowProgress(true, description);
        last_progress = -2;
    }
}

bool ModelController::isKeyNameVisual(QString key_name){
    QStringList visual_keynames;
    visual_keynames << "x" << "y" << "width" <<  "height" << "isExpanded";
    return visual_keynames.contains(key_name);
}

bool ModelController::importGraphML(QString document, Node *parent)
{
    //Lookup for key's ID to Key* object
    QHash <QString, Key*> key_hash;

    QStack<QString> node_ids;
    QStack<QString> edge_ids;
    QStack<QString> unique_entity_ids;

    QHash<QString, TempEntity*> entity_hash;

    if(!parent){
        //Set parent as Model item.
        parent = model;
    }

    if(!isGraphMLValid(document)){
        emit Notification(MODEL_SEVERITY::ERROR, "GraphML is invalid!");
        return false;
    }

    int error_count = 0;

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);

    bool link_id = isModelAction(MODEL_ACTION::UNDO) || isModelAction(MODEL_ACTION::REDO);
    bool reset_position = isModelAction(MODEL_ACTION::PASTE);
    bool partial_import = isModelAction(MODEL_ACTION::PASTE) || link_id;
    auto current_entity = new TempEntity(GRAPHML_KIND::NODE);
    bool show_progress = !isModelAction(MODEL_ACTION::NONE);
    bool process_uuids = isModelAction(MODEL_ACTION::IMPORT);

    current_entity->setID(parent->getID());


    if(show_progress){
        ProgressUpdated_("Parsing Project");
        ProgressChanged_(-1);
    }

    while(xml.readNext() != QXmlStreamReader::EndDocument){
        //Check what kind we are dealing with
        GRAPHML_KIND kind = getGraphMLKindFromString(xml.name());

        //Handle the elements
        if(xml.isStartElement()){
            switch(kind){
                case GRAPHML_KIND::KEY:{
                    auto key_id = getXMLAttribute(xml, "id");
                    auto key_name = getXMLAttribute(xml, "attr.name");
                    auto key_type = Key::getTypeFromGraphML(getXMLAttribute(xml, "attr.type"));

                    auto key = entity_factory->GetKey(key_name, key_type);
                    if(key){
                        if(!key_hash.contains(key_id)){
                            key_hash.insert(key_id, key);
                        }else{
                            qCritical() << "ImportGraphML: Duplicate key id '" << key_id << "'";
                            error_count ++;
                            continue;
                        }
                    }else{
                        qCritical() << "ImportGraphML: Couldn't construct key id '" << key_id << "'";
                        error_count ++;
                        continue;
                    }
                    break;
                }
                case GRAPHML_KIND::DATA:{
                    auto key_id = getXMLAttribute(xml, "key");
                    auto key = key_hash.value(key_id, 0);

                    if(current_entity && key){
                        auto key_name = key->getName();
                        auto value = xml.readElementText();
                        
                        if(key_name == "uuid"){
                            //Run the UUID through
                            value = ExportIDKey::GetUUIDOfValue(value);
                            unique_entity_ids.push_back(current_entity->getIDStr());
                        }else if(key_name == "medea_version"){
                            if(value != APP_VERSION()){
                                QString message = "Model was created in MEDEA v" % value % ". Some functionality may have changed.";
                                emit Notification(MODEL_SEVERITY::INFO, message);
                            }
                        }
                        //Add the data to the entity
                        current_entity->addData(key_name, value);
                    }else{
                        qCritical() << "ImportGraphML: Couldn't attach Data with key id: '" << key_id << "'";
                        error_count ++;
                        continue;
                    }
                    break;
                }
                case GRAPHML_KIND::EDGE:
                case GRAPHML_KIND::NODE:{
                    auto id = getXMLAttribute(xml, "id");
                    auto entity = new TempEntity(kind, current_entity);
                    entity->setIDStr(id);
                    entity->setLineNumber(xml.lineNumber());

                    if(!entity_hash.contains(id)){
                        //Insert into the hash.
                        entity_hash.insert(id, entity);
                    }else{
                        qCritical() << "ImportGraphML: Duplicate Entity Ids found '" << id << "'";
                        error_count ++;
                        continue;
                    }

                    if(kind == GRAPHML_KIND::NODE){
                        node_ids.push(id);
                    }else if(kind == GRAPHML_KIND::EDGE){
                        //Handle Source/Target for edges.
                        entity->setSourceIDStr(getXMLAttribute(xml, "source"));
                        entity->setTargetIDStr(getXMLAttribute(xml, "target"));

                        edge_ids.push(id);
                    }
                    //Set the Item as the current Entity.
                    current_entity = entity;
                    //TODO reset position
                }
                default:
                    break;
            }
        }

        if(xml.isEndElement()){
            //For Nodes/Edges, step up a parent.
            if(kind == GRAPHML_KIND::NODE || kind == GRAPHML_KIND::EDGE){
                current_entity = current_entity->getParent();
            }
        }
    }

    QList<Entity*> to_remove;
    
    //Handle unique ids
    for(auto id : unique_entity_ids){
        auto entity = entity_hash.value(id, 0);
        //Get the UUID
        auto uuid = entity->getDataValue("uuid").toString();
        auto kind = entity->getDataValue("kind").toString();
        bool export_uuid = process_uuids;

        //Always handle uuids for HardwareNodes/Clusters
        if(!export_uuid && entity->isNode()){
            NODE_KIND node_kind = entity_factory->getNodeKind(kind);
            switch(node_kind){
                case NODE_KIND::HARDWARE_CLUSTER:
                case NODE_KIND::HARDWARE_NODE:
                    export_uuid = true;
                    break;
                default:
                    break;
            }
        }


        //Handle UUIDS
        if(export_uuid){
            //If we have a uuid, we should set the entity as read-only
            entity->addData("readOnly", true);

            //Lookup the entity in the 
            auto matched_entity = entity_factory->GetEntityByUUID(uuid);
            
            if(matched_entity && matched_entity->isNode()){
                auto matched_node = (Node*) matched_entity;
                //Produce a notification for updating shared_datatypes
                
                if(matched_node->getNodeKind() == NODE_KIND::SHARED_DATATYPES){
                    auto version = entity->getDataValue("version").toString();
                    auto old_version = matched_entity->getDataValue("version").toString();
                    auto old_label = matched_entity->getDataValue("label").toString();
                    if(!version.isEmpty() && !old_version.isEmpty()){
                        QString message = "Updated SharedDatatypes '" % old_label % "' from '" % old_version % "' to '" % version % "'";
                        emit Notification(MODEL_SEVERITY::INFO, message, matched_entity->getID());
                    }
                }
                //Set the entity to use this.
                entity->setID(matched_entity->getID());

                if(matched_entity->isNode()){
                    auto matched_node = (Node*) matched_entity;
                    
                    //Create a list of all required uuids this entity we are loading requires
                    QStringList required_uuids;
                    for(auto child : entity->getChildren()){
                        required_uuids << child->getDataValue("uuid").toString();
                    }
                    
                    //Remove all visual data.
                    for(auto key_name : entity->getKeys()){
                        if(isKeyNameVisual(key_name)){
                            entity->removeData(key_name);
                        }
                    }

                    //Compare the children we already have in the Model to the children we need to import. Remove any which aren't needed
                    for(auto child : matched_node->getChildren(0)){
                        auto child_uuid = child->getDataValue("uuid").toString();
                        if(!required_uuids.contains(child_uuid)){
                            to_remove.push_back(child);
                        }
                    }
                }
            }
        }
    }

    //Get the ordered list of entities to remove
    to_remove = getOrderedEntities(to_remove);
    destructEntities(to_remove);
    
    //This is will update as a percentage
    double entities_total_perc = entity_hash.size() / 100.0;
    double entities_made = 0;

    if(show_progress){
        ProgressUpdated_("Constructing Nodes");
    }

    //Construct all nodes
    for(auto id : node_ids){
        auto entity = entity_hash.value(id, 0);

        if(!entity || !entity->isNode()){
            qCritical() << "ImportGraphML: Couldn't find Node with ID:" << id;
            error_count ++;
            continue;
        }

        //Get the parent
        auto parent_entity = entity->getParent();

        //Check if our parent_entity exists
        if(!parent_entity || !parent_entity->gotID()){
            qCritical() << "ImportGraphML: Node with ID:" << id << "has no Parent Entity";
            error_count ++;
            continue;
        }

        //This is the top most element
        if(parent_entity == current_entity && reset_position){
            //Remove all visual data.
            for(auto key_name : entity->getKeys()){
                if(isKeyNameVisual(key_name)){
                    entity->removeData(key_name);
                }
            }
        }
        
        auto parent_node = entity_factory->GetNode(parent_entity->getID());
        auto parent_kind = parent_node->getNodeKind();
        auto kind = entity_factory->getNodeKind(entity->getKind());

        Node* node = 0;

        if(entity->gotID()){
            //Get the already existant node.
            node = entity_factory->GetNode(entity->getID());
        }else{

            //Construct a new node if we haven't got one yet, this can return nodes which are already constructed.
            if(link_id && entity->gotPreviousID()){
                node = construct_node(parent_node, kind, entity->getPreviousID());
            }else{
                node = construct_node(parent_node, kind);
            }

            //If we have matched something which is already got a parent node we shouldn't overwrite its data
            //This will stop any data stored in the aspects getting overwritten
            if(node && node->getParentNode() && isModelAction(MODEL_ACTION::IMPORT)){
                entity->clearData();
            }
        }

        if(node){
            //If we aren't the Model, check if we have a parent_node
            bool got_parent = node->getParentNode() || kind == NODE_KIND::MODEL;
            bool need_to_gui = false;
            if(!got_parent){
                //If it's not got a parent, set it.
                got_parent = attachChildNode(parent_node, node, false);
                need_to_gui = true;
            }

            if(got_parent){
                auto node_id = node->getID();

                if(need_to_gui){
                    storeNode(node);
                }
                for(auto key_name : entity->getKeys()){
                    auto value = entity->getDataValue(key_name);
                    setData_(node, key_name, value, true);
                }

                //Set Actual ID
                entity->setID(node->getID());

            }else{
                //Destroy!
                entity_factory->DestructEntity(node);
                node = 0;
            }
        }
        if(!node){
            QString message = "Cannot create Node: '" % entity->getKind() % "' from document at line #" % QString::number(entity->getLineNumber()) % ".";
            error_count ++;
            emit Notification(MODEL_SEVERITY::ERROR, message);
        }

        if(show_progress){
            ProgressChanged_(++entities_made / entities_total_perc);
        }
    }


    QMultiMap<EDGE_KIND, TempEntity*> edge_map;


    for(auto id : edge_ids){
        auto entity = entity_hash.value(id, 0);

        if(!entity || !entity->isEdge()){
            qCritical() << "ImportGraphML: Couldn't find Edge with ID:" << id;
            error_count ++;
            continue;
        }

        //Try find the Entity we should be creating.
        auto src_entity = entity_hash.value(entity->getSourceIDStr(), 0);
        auto dst_entity = entity_hash.value(entity->getTargetIDStr(), 0);

        Node* src_node = 0;
        Node* dst_node = 0;

        if(src_entity && src_entity->gotID()){
            src_node = entity_factory->GetNode(src_entity->getID());
        }else if(partial_import){
            //Try and see if the integer version of the ID stored in the GraphML Still exists in the model.
            src_node = entity_factory->GetNode(entity->getSourceIDInt());
        }

        if(dst_entity && dst_entity->gotID()){
            dst_node = entity_factory->GetNode(dst_entity->getID());
        }else if(partial_import){
            //Try and see if the integer version of the ID stored in the GraphML Still exists in the model.
            dst_node = entity_factory->GetNode(entity->getTargetIDInt());
        }

        if(src_node && dst_node){
            //Set destination.
            entity->setSourceID(src_node->getID());
            entity->setTargetID(dst_node->getID());

            EDGE_KIND edge_kind = entity_factory->getEdgeKind(entity->getKind());

            if(edge_kind != EDGE_KIND::NONE){
                entity->appendEdgeKind(edge_kind);
            }else{
                //If the edge class stored in the model is invalid we should try all of the edge classes these items can take, in order.
                entity->appendEdgeKinds(getPotentialEdgeClasses(src_node, dst_node));
            }

            if(entity->gotEdgeKind()){
                auto next_edge_kind = entity->getEdgeKind();
                //Insert the item in the lookup
                edge_map.insertMulti(next_edge_kind, entity);
            }else{
                QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". No valid edge kinds.";
                emit Notification(MODEL_SEVERITY::ERROR, message);
                error_count ++;
            }
        }else{
            QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". Missing Source or destination.";
            emit Notification(MODEL_SEVERITY::ERROR, message);
            error_count ++;
        }
    }

    if(show_progress){
        ProgressUpdated_("Constructing Edges");
    }

    int edge_itterations = 0;
    auto edge_kind_keys = GetEdgeOrderIndexes();

    while(!edge_map.isEmpty()){
        QList<TempEntity*> edges;
        QList<TempEntity*> unconstructed_edges;
        
        int constructed_edges = 0;
      EDGE_KIND edge_kind = EDGE_KIND::NONE;
        
        //Find the first index which still has edges left
        for(auto kind : edge_kind_keys){
            if(edge_map.contains(kind)){
                edges = edge_map.values(kind);
                edge_kind = kind;
                break;
            }
        }

        //Reverse the edges, (QMap inserts in a stack form LIFO)
        std::reverse(edges.begin(), edges.end());

        for(auto entity : edges){
            //Remove this edge from the map
            edge_map.remove(edge_kind, entity);
            
            //Get the src/dst node from the edge
            auto src_node = entity_factory->GetNode(entity->getSourceID());
            auto dst_node = entity_factory->GetNode(entity->getTargetID());

            Edge* edge = 0;
            if(src_node && dst_node){
                //Check if we have an edge first
                edge = src_node->getEdgeTo(dst_node, edge_kind);

                if(!edge){
                    //Do some checks
                    //Construct a new node if we haven't got one yet, this can return nodes which are already constructed.
                    if(link_id && entity->gotPreviousID()){
                        edge = construct_edge(edge_kind, src_node, dst_node, entity->getPreviousID());
                    }else{
                        edge = construct_edge(edge_kind, src_node, dst_node);
                    }
                }

                if(edge){
                    for(auto key_name : entity->getKeys()){
                        auto value = entity->getDataValue(key_name);
                        setData_(edge, key_name, value, true);
                    }

                    //Set Actual ID
                    entity->setID(edge->getID());
                }
            }else{
                //Can't find source/destination
                QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". No endpoints.";
                emit Notification(MODEL_SEVERITY::ERROR, message);
                error_count ++;
            }

            if(edge){
                if(show_progress){
                    ProgressChanged_(++entities_made / entities_total_perc);
                }
                constructed_edges++;
            }else{
                //If we couldn't construct an edge, push it onto the list of edges we couldn't construct
                unconstructed_edges.push_back(entity);
            }
        }

        //Go through the list of unconstructed edges.
        for(auto entity : unconstructed_edges){
            //If no edges were constructed this pass, it means no edges can be made of this EDGE_KIND
            if(constructed_edges == 0){
                //Remove the current edgeKind, so we can try the next (If it has one)
                entity->removeEdgeKind(entity->getEdgeKind());
            }

            if(entity->gotEdgeKind()){
                auto next_edge_kind = entity->getEdgeKind();
                //Reinsert back into the map (Goes to the start)
                edge_map.insertMulti(next_edge_kind, entity);
            }else{
                //Can't find source/destination
                QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". No valid edge_kinds.";
                emit Notification(MODEL_SEVERITY::ERROR, message);
                error_count ++;
            }
        }
        edge_itterations ++;
    }

    if(show_progress){
        ProgressChanged_(100);
    }

    if(edge_ids.size() > 0){
        //qCritical() << "Imported: #" << edge_ids.size() << " Edges in " << edge_itterations << " Itterations.";
    }

    for(auto entity : entity_hash){
        delete entity;
    }
    delete current_entity;
    
    return error_count == 0;
}


HistoryAction ModelController::getNewAction(GRAPHML_KIND kind)
{
    HistoryAction action;
    action.Action.id = currentActionID;
    action.Action.name = currentAction;
    action.Action.action_id = currentActionItemID++;
    action.Action.kind = kind;
    return action;
}

void ModelController::setProjectModified(bool dirty)
{
    if(project_modified != dirty){
        project_modified = dirty;
        emit ProjectModified(dirty);
    }
}

void ModelController::setProjectPath(QString path)
{
    if(project_path != path){
        project_path = path;
        emit ProjectFileChanged(project_path);
    }
}


QSet<SELECTION_PROPERTIES> ModelController::getSelectionProperties(int active_id, QList<int> ids){
    QReadLocker lock(&lock_);
    QSet<SELECTION_PROPERTIES> properties;

    auto item = entity_factory->GetEntity(active_id);

    //Got a list of entites
    auto items = getOrderedEntities(ids);

    if(canCut(items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CUT);
    }

    if(canCopy(items)){
        properties.insert(SELECTION_PROPERTIES::CAN_COPY);
    }

    if(canRemove(items)){
        properties.insert(SELECTION_PROPERTIES::CAN_REMOVE);
    }

    if(canPaste(items)){
        properties.insert(SELECTION_PROPERTIES::CAN_PASTE);
    }

    if(canReplicate(items)){
        properties.insert(SELECTION_PROPERTIES::CAN_REPLICATE);
    }

    //Check Active Selection
    if(item){
        auto label = item->getData("label");
        if(label && !item->isReadOnly() && !label->isProtected()){
            properties.insert(SELECTION_PROPERTIES::CAN_RENAME);
        }

        if(item->isNode()){
            auto node = (Node*) item;

            if(getDefinition(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_DEFINITION);
            }

            if(getImplementation(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_IMPLEMENTATION);
            }

            if(getInstances(node).size() > 0){
                properties.insert(SELECTION_PROPERTIES::GOT_INSTANCES);
            }

            if(node->getEdges(0, EDGE_KIND::NONE).size() > 0){
                properties.insert(SELECTION_PROPERTIES::GOT_EDGES);
            }
        }
    }

    return properties;
}

bool ModelController::canCut(QList<Entity *> selection)
{
    if(canRemove(selection)){
        for(auto item : selection){
            if(item->isNode()){
                Node* node = (Node*) item;
               
                //Dont allow copy from any definitions, except 
                if(node->isDefinition()){
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}



bool ModelController::canCopy(QList<Entity *> selection)
{
    bool valid = !selection.isEmpty();
    Node* parent = 0;

    for(auto item : selection){
        if(item->isNode()){
            Node* node = (Node*) item;
            if(!canDeleteNode(node)){
                return false;
            }

            auto current_parent = node->getParentNode();

            //Dont allow copy from any definitions, except 
            if(node->isDefinition()){
                switch(node->getNodeKind()){
                    case NODE_KIND::AGGREGATE:
                    case NODE_KIND::COMPONENT:
                        break;
                    default:
                        return false;
                }
            }


            //Don't allow selection from different parents to be copyable
            if(!parent){
                parent = current_parent;
            }else if(parent != current_parent){
                return false;
            }
        }
    }

    return valid;
}

bool ModelController::canPaste(QList<Entity *> selection)
{
    if(selection.size() == 1){
        Entity* item = selection.first();

        if(item && item->isNode()){
            auto node = (Node*) item;

            if(!node->isReadOnly()){
                if(node->isInstanceImpl() && node->getNodeKind() != NODE_KIND::COMPONENT_IMPL){
                    return false;
                }

                if(node->isDefinition()){
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}


bool ModelController::canReplicate(QList<Entity *> selection)
{
    if(canCopy(selection)){
        //Get the first item, check if we can paste into it
        auto first = selection.first();
        return canPaste(QList<Entity*>{first});
    }
    return false;
}

bool ModelController::canRemove(QList<Entity *> selection)
{
    if(selection.isEmpty()){
        return false;
    }

    for(auto entity : selection){
        if(entity->isNode()){
            auto node = (Node*) entity;
            auto parent_node = node->getParentNode();

            if(!canDeleteNode(node)){
                return false;
            }

            if(parent_node){
                auto definition = node->getDefinition();
                if(node->isImpl() && definition){
                    //Only allowed to delete OutEventPortImpls
                    if(node->getNodeKind() != NODE_KIND::OUTEVENTPORT_IMPL){
                        return false;
                    }
                }

                if(node->isInstance() && definition && parent_node->isInstance()){
                    return false;
                }

                if(node->isReadOnly() && parent_node->isReadOnly()){
                    return false;
                }
            }
        }else if(entity->isEdge()){
            Edge* edge = (Edge*) entity;

            if(edge->isReadOnly()){
                return false;
            }
        }
    }
    return true;
}

bool ModelController::canUndo()
{
    QReadLocker lock(&lock_);
    return !undo_stack.isEmpty();
}

bool ModelController::canRedo()
{
    QReadLocker lock(&lock_);
    return !redo_stack.isEmpty();
}

QString ModelController::getProjectPath()
{
    QReadLocker lock(&lock_);
    return project_path;
}

bool ModelController::isProjectSaved()
{
    QReadLocker lock(&lock_);
    return project_modified == false;
}

int ModelController::getProjectActionCount()
{
    QReadLocker lock(&lock_);
    return currentActionItemID;
}

Node* ModelController::getImplementation(Node* node){
    
    auto def = getDefinition(node);
    if(node){
        //If we have a Definition (And the node isn't an Impl), use it to check for impls, else use the node
        node = (def && !node->isImpl()) ? def : node;
        for(auto impl : node->getImplementations()){
            return impl;
        }
    }
    return 0;
}

Node* ModelController::getDefinition(Node* node){
    if(node){
        return node->getDefinition(true);
    }
    return 0;
}

QList<Node*> ModelController::getInstances(Node* node){
    QList<Node*> instances;
    if(node){
        if(node->isImpl()){
            //Impls have instances 
            node = getDefinition(node);
        }
        
        if(node){
            instances = node->getInstances();
        }
    }
    return instances;
}


int ModelController::getDefinition(int id)
{
    QReadLocker lock(&lock_);
    auto def = getDefinition(entity_factory->GetNode(id));
    return def ? def->getID() : -1;
}

int ModelController::getImplementation(int id)
{
    QReadLocker lock(&lock_);
    auto impl = getImplementation(entity_factory->GetNode(id));
    return impl ? impl->getID() : -1;
}

bool ModelController::isUserAction()
{
    if(isModelAction(MODEL_ACTION::UNDO) || isModelAction(MODEL_ACTION::REDO) || isModelAction(MODEL_ACTION::OPEN)){
        return false;
    }else{
        return true;
    }
}

void ModelController::setModelAction(MODEL_ACTION action){
    this->model_actions.insert(action);
}

void ModelController::unsetModelAction(MODEL_ACTION action){
    this->model_actions.remove(action);
}

bool ModelController::isModelAction(MODEL_ACTION action){
    return model_actions.contains(action);
}