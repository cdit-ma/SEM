#include "modelcontroller.h"
#include <QDebug>
#include <algorithm>
#include <QDateTime>

#include <QDir>
#include <QStringBuilder>
#include <QThread>
#include <QFile>
#include <QElapsedTimer>
#include <QXmlStreamReader>

#include "version.h"

#include "entityfactory.h"
#include "entityfactorybroker.h"
#include "entityfactoryregistrybroker.h"
#include "tempentity.h"

#include "Entities/edge.h"
#include "Entities/data.h"
#include "Entities/node.h"
#include "Entities/key.h"
#include "Entities/Keys/exportidkey.h"

#include "Entities/InterfaceDefinitions/eventport.h"
#include "Entities/InterfaceDefinitions/aggregate.h"
#include "Entities/InterfaceDefinitions/datanode.h"

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

inline QString getXMLAttribute(const QXmlStreamReader &xml, const QString& attribute)
{
    //Get the Attributes of the current XML entity.
    const auto &attributes = xml.attributes();

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
    qRegisterMetaType<QSet<EDGE_DIRECTION> >("QSet<EDGE_DIRECTION>");
    qRegisterMetaType<DataUpdate>("DataUpdate");
    
}

void ModelController::ConnectViewController(ViewControllerInterface* view_controller){
    if(view_controller){
        connect(view_controller, &ViewControllerInterface::SetupModelController, this, &ModelController::SetupController, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ImportProjects, this, &ModelController::importProjects, Qt::QueuedConnection);

        connect(view_controller, &ViewControllerInterface::TriggerAction, this, &ModelController::triggerAction, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::SetData, this, &ModelController::setData, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::RemoveData, this, &ModelController::removeData, Qt::QueuedConnection);

        connect(view_controller, &ViewControllerInterface::ConstructNodeAtIndex, this, &ModelController::constructNodeAtIndex, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructNodeAtPos, this, &ModelController::constructNodeAtPos, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructConnectedNodeAtIndex, this, &ModelController::constructConnectedNodeAtIndex, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructConnectedNodeAtPos, this, &ModelController::constructConnectedNodeAtPos, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructEdges, this, &ModelController::constructEdges, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::DestructEdges, this, &ModelController::destructEdges, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::DestructAllEdges, this, &ModelController::destructAllEdges, Qt::QueuedConnection);
        

        connect(view_controller, &ViewControllerInterface::Undo, this, &ModelController::undo, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Redo, this, &ModelController::redo, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Delete, this, &ModelController::remove, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Paste, this, &ModelController::paste, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Replicate, this, &ModelController::replicate, Qt::QueuedConnection);
    }
}

bool ModelController::SetupController(QString file_path)
{
    QWriteLocker lock(&lock_);
    setupModel();
    loadWorkerDefinitions();
    clearHistory();

    auto file = readFile(file_path);
    bool success = true;
    if(file.first){
        setModelAction(MODEL_ACTION::OPEN);
        success = importGraphML(file.second, model);
        unsetModelAction(MODEL_ACTION::OPEN);
        
        if(success){
            //Update the project filePath
            setProjectPath(file_path);
        }
    }else{
        emit ProjectFileChanged("Untitled Project");
    }

    //Clear the Undo/Redo History.
    clearHistory();
    setProjectModified(!success);
    emit ModelReady(true);
    return success;
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
        //QStringList extensions{"*memory*.worker"};

        setModelAction(MODEL_ACTION::IMPORT);
        for(auto dir : worker_directories){
            for(auto file : dir.entryList(extensions)){
                auto file_path = dir.absolutePath() + "/" + file;
                auto data = readFile(file_path);
                bool success = importGraphML(data.second, workerDefinitions);
                if(!success){
                    QString title = "Error Importing Worker Definition";
                    QString description = "loadWorkerDefinitions(): Importing '" % file_path % "' failed";
                    emit Notification(MODEL_SEVERITY::WARNING, title, description);
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

QString ModelController::exportGraphML(QList<Entity*> selection, bool all_edges, bool functional_export){
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
            for(auto edge : node->getEdges(-1)){
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
    
    
    auto key_list = keys.toList();
    std::sort(key_list.begin(), key_list.end(), GraphML::SortByID);
    
    //Define the key graphml
    for(auto key : key_list){
        xml += key->toGraphML(1, functional_export);
    }

    {
        xml +="\n\t<graph edgedefault=\"directed\" id=\"parentGraph0\">\n";
        for(auto entity : top_nodes){
            xml += entity->toGraphML(2, functional_export);
        }
        
        //Sort the edges to be lowest to highest IDS for determinism
        auto edge_list = edges.toList();
        std::sort(edge_list.begin(), edge_list.end(), [](const Edge* e1, const Edge* e2){
            return e1->getID() > e2->getID();
        });

        for(auto edge : edge_list){
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
                xml += edge->toGraphML(2, functional_export);
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
        case EDGE_KIND::DATA:
            return 2;
        case EDGE_KIND::QOS:
            return 3;
        case EDGE_KIND::ASSEMBLY:
            return 4;
        case EDGE_KIND::DEPLOYMENT:
            return 5;
        default:
            return 100;
    }
}

QList<EDGE_KIND> ModelController::GetEdgeOrderIndexes(){
    QList<EDGE_KIND> indices;

    indices << EDGE_KIND::DEFINITION;
    indices << EDGE_KIND::AGGREGATE;
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
    bool data_changed = false;
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
            data_changed = entity->setDataValue(key_name, value);
        }else{
            //Set the type
            action.Action.type = ACTION_TYPE::CONSTRUCTED;
            auto key = entity_factory->GetKey(key_name, value.type());
            auto data = entity_factory->AttachData(entity, key, ProtectedState::IGNORED, value);
        }

        if(data_changed){
            action.Data.new_value = entity->getDataValue(key_name);
            addActionToStack(action, add_action);
        }
    }
    return data_changed;
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




Node* ModelController::get_persistent_node(NODE_KIND node_kind){
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

void ModelController::set_persistent_node(Node* node){
    if(node){
        auto node_kind = node->getNodeKind();
        protected_nodes << node;
        switch(node_kind){
        case NODE_KIND::MODEL:{
            model = node;
            break;
        }
        case NODE_KIND::INTERFACE_DEFINITIONS:{
            interfaceDefinitions = node;
            break;
        }
        case NODE_KIND::BEHAVIOUR_DEFINITIONS:{
            behaviourDefinitions = node;
            break;
        }
        case NODE_KIND::WORKER_DEFINITIONS:{
            workerDefinitions = node;
            break;
        }
        case NODE_KIND::ASSEMBLY_DEFINITIONS:{
            assemblyDefinitions = node;
            break;
        }
        case NODE_KIND::DEPLOYMENT_DEFINITIONS:{
            deploymentDefinitions = node;
            break;
        }
        case NODE_KIND::HARDWARE_DEFINITIONS:{
            hardwareDefinitions = node;
            break;
        }
        default:
            break;
        }
    }
}

Node* ModelController::construct_node(Node* parent_node, NODE_KIND node_kind, int index){
    auto persistent_node = get_persistent_node(node_kind);
    if(!persistent_node){
        //Construct node with default data
        auto node = entity_factory->CreateNode(node_kind, true); 
        if(index != -1){
            node->setDataValue("index", index);
        }
        return node;
    }
    return persistent_node;
}

Node* ModelController::construct_child_node(Node* parent_node, NODE_KIND node_kind, int index, bool notify_view){
    Node* node = 0;
    if(parent_node){
        //Don't construct nodes:
        node = construct_node(parent_node, node_kind, index);
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

void ModelController::addDependantsToDependants(Node* parent_node, Node* source){
    if(parent_node && source){
        if(isUserAction() && source->isDefinition()){
            for(auto dependant : parent_node->getDependants()){
                QList<QSet<NODE_KIND> > dependant_kind_list;
                
                if(dependant->isInstance()){
                    dependant_kind_list.append(source->getInstanceKinds());
                }else{
                    dependant_kind_list.append(source->getImplKinds());
                    dependant_kind_list.append(source->getInstanceKinds());
                }


                

                for(auto &dependant_kinds : dependant_kind_list){
                    bool constructed_dependant = false;
                    //Try Either
                    for(auto kind : dependant_kinds){
                        auto dependant_child = construct_connected_node(dependant, kind, source, EDGE_KIND::DEFINITION);
                        if(dependant_child){
                            constructed_dependant = true;
                            break;
                        }
                    }
                    if(constructed_dependant){
                        break;
                    }
                }
            }
        }
    }
}

Node* ModelController::construct_connected_node(Node* parent_node, NODE_KIND node_kind, Node* destination, EDGE_KIND edge_kind, int index){
    auto source = construct_node(parent_node, node_kind, index);
    bool success = false;
    if(source){
        //Try attach
        if(!source->getParentNode()){
            attachChildNode(parent_node, source, false);
        }

        
        if(source->getParentNode() == parent_node){
            auto edge = construct_edge(edge_kind, source, destination, false);
            if(edge){
                storeNode(source);
                storeEdge(edge);
                success = true;
            }else{
                qCritical() << "COULDN'T MAKE EDGE BETWEEN: " << source->toString() << " < - > " << destination->toString();
            }
        }
    }

    //Make sure we add Dependants
    addDependantsToDependants(parent_node, source);

    if(!success && source){
        entity_factory->DestructEntity(source);
        source = 0;
    }
    return source;
}

Node* ModelController::constructNode(Node* parent_node, NODE_KIND kind, int index){
    //Add undo step
    triggerAction("Constructing Child Node");
    
    Node* node = 0;
    switch(kind){
        case NODE_KIND::COMPONENT:{
            node = construct_component_node(parent_node, index);
            break;
        }
        default:{
            node = construct_child_node(parent_node, kind, index);
            break;
        }
    }
    
    if(node){
        addDependantsToDependants(parent_node, node);
    }
    return node;
}

//SIGNAL
void ModelController::constructNodeAtPos(int parent_id, NODE_KIND kind, QPointF pos)
{
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto node = constructNode(parent_node, kind);

    if(node && !pos.isNull()){
        setData_(node, "x", pos.x());
        setData_(node, "y", pos.y());
    }
    emit ActionFinished();
}

void ModelController::constructNodeAtIndex(int parent_id, NODE_KIND kind, int index)
{
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto node = constructNode(parent_node, kind, index);

    if(node && index >= 0){
        setData_(node, "index", index);
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
        for(auto src : src_nodes){
            for(auto dst : dst_nodes){

                auto edge = construct_edge(edge_kind, src, dst);
                if(!edge){
                    success = false;
                    break;
                }
            }
        }
    }
    emit ActionFinished();
}


void ModelController::destructEdges(QList<int> src_ids, QList<int> dst_ids, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing edges");

    QList<Entity*> edges;
    
    for(auto src_id : src_ids){
        auto src = entity_factory->GetNode(src_id);

        for(auto dst_id : dst_ids){
            auto dst = entity_factory->GetNode(dst_id);
            if(src && dst){
                auto edge = src->getEdgeTo(dst, edge_kind);
                if(edge){
                    edges << edge;
                }
            }
        }
    }
    destructEntities(edges);
    emit ActionFinished();
}

void ModelController::destructAllEdges(QList<int> src_ids, EDGE_KIND edge_kind, QSet<EDGE_DIRECTION> edge_directions)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing All edges");

    QList<Entity*> edges;

    for(auto src_id : src_ids){
        auto src = entity_factory->GetNode(src_id);
        for(auto edge: src->getEdgesOfKind(edge_kind, 0)){
            if(edge){
                auto is_source = edge->getSource() == src;
                
                auto direction_to_delete = is_source ? EDGE_DIRECTION::TARGET : EDGE_DIRECTION::SOURCE;

                if(edge_directions.contains(direction_to_delete)){
                    edges << edge;
                }
            }
        }
    }
    destructEntities(edges);
    emit ActionFinished();
}

Node* ModelController::construct_component_node(Node* parent, int index){
    if(parent){
        triggerAction("Constructing Component");

        auto node = construct_child_node(parent, NODE_KIND::COMPONENT, index);
        
        if(node){
            auto impl = construct_connected_node(behaviourDefinitions, NODE_KIND::COMPONENT_IMPL, node, EDGE_KIND::DEFINITION);
            return node;
        }
    }
    return 0;
}

void ModelController::constructConnectedNodeAtPos(int parent_id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, QPointF pos){
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto dst_node = entity_factory->GetNode(dst_id);
    auto node = constructConnectedNode(parent_node, node_kind, dst_node, edge_kind);

    if(node && !pos.isNull()){
        setData_(node, "x", pos.x());
        setData_(node, "y", pos.y());
    }
    emit ActionFinished();
}
void ModelController::constructConnectedNodeAtIndex(int parent_id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, int index){
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto dst_node = entity_factory->GetNode(dst_id);
    auto node = constructConnectedNode(parent_node, node_kind, dst_node, edge_kind);

    if(node && index >= 0){
        setData_(node, "index", index);
    }
    emit ActionFinished();
}

Node* ModelController::constructConnectedNode(Node* parent_node, NODE_KIND node_kind, Node* dst_node, EDGE_KIND edge_kind, int index){
    Node* node = 0;
    if(parent_node && dst_node){
        triggerAction("Constructed Connected Node");
        node = construct_connected_node(parent_node, node_kind, dst_node, edge_kind, index);
    }
    return node;
}


Edge* ModelController::construct_edge(EDGE_KIND edge_kind, Node *src, Node *dst, bool notify_view)
{
    Edge* edge = entity_factory->CreateEdge(src, dst, edge_kind);

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
QString ModelController::copy(QList<int> ids)
{
    QReadLocker lock(&lock_);
    auto selection = getOrderedEntities(ids);

    if(canCopy(selection)){
        auto value = _copy(selection);

        return value;
    }
    return "";
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

QString ModelController::cut(QList<int> ids)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(ids);
    QString data;
    if(canCut(selection)){
        data = exportGraphML(selection, true);
        emit triggerAction("Cutting Selection");
        destructEntities(selection);
    }
    emit ActionFinished();
    return data;
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



QMap<EDGE_DIRECTION, int> ModelController::getConnectableNodes(QList<int> src_ids, EDGE_KIND edge_kind){
    QMap<EDGE_DIRECTION, int> id_map;

    QReadLocker lock(&lock_);

    auto node_map = _getConnectableNodes(getNodes(src_ids), edge_kind);
    for(const auto& key : node_map.uniqueKeys()){
        for(const auto& value : node_map.values(key)){
            id_map.insertMulti(key, value->getID());
        }
    }
    return id_map;
}

QMap<EDGE_DIRECTION, Node*> ModelController::_getConnectableNodes(QList<Node*> src_nodes, EDGE_KIND edge_kind){
    QMap<EDGE_DIRECTION, Node*> node_map;
    
    bool srcs_accept_source_edge = true;
    bool srcs_accept_target_edge = true;
    
    //Check if they can all accept edges of the kind we care about.
    for(auto src : src_nodes){
        if(!src->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::SOURCE)){
            srcs_accept_source_edge = false;
            break;
        }
        if(!src->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::TARGET)){
            srcs_accept_target_edge = false;
        }
    }

    //Only itterate if we have nodes
    if((srcs_accept_source_edge || srcs_accept_target_edge) && src_nodes.size()){
        if(srcs_accept_source_edge){
            
            for(auto dst : entity_factory->GetNodesWhichAcceptEdgeKinds(edge_kind, EDGE_DIRECTION::TARGET)){
                bool src2dst_valid = true;

                for(auto src : src_nodes){
                    //Only check if for edge adoption if true, otherwise something can't adopt and thus no need to check
                    src2dst_valid = src2dst_valid ? src->canAcceptEdge(edge_kind, dst) : false;
                }
                
                if(src2dst_valid){
                    node_map.insertMulti(EDGE_DIRECTION::TARGET, dst);
                }
            }
        }
        if(srcs_accept_target_edge){
            for(auto dst : entity_factory->GetNodesWhichAcceptEdgeKinds(edge_kind, EDGE_DIRECTION::SOURCE)){
                bool dst2src_valid = true;

                for(auto src : src_nodes){
                    dst2src_valid = dst2src_valid ? dst->canAcceptEdge(edge_kind, src) : false;
                }
                
                if(dst2src_valid){
                    node_map.insertMulti(EDGE_DIRECTION::SOURCE, dst);
                }
            }
        }
    }
    return node_map;
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
    
    auto entity = entity_factory->GetEntity(id);
    if(entity){
        auto data = entity->getData(key_name);
        if(data){
            valid_values = data->getValidValues();
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

VIEW_ASPECT ModelController::getNodeViewAspect(int ID){
    QReadLocker lock(&lock_);
    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;
    auto node = entity_factory->GetNode(ID);
    
    if(node){
        aspect = node->getViewAspect();
    }
    return aspect;
}


QList<DataUpdate> ModelController::getEntityDataList(int ID){
    QList<DataUpdate> data_list;
    QReadLocker lock(&lock_);

    auto entity = entity_factory->GetEntity(ID);
    if(entity){
        for(auto data : entity->getData()){

            data_list += DataUpdate({data->getKeyName(), data->getValue(), data->isProtected()});
        }
    }
    return data_list;
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

QSet<NODE_TYPE> ModelController::getNodesTypes(int ID){
    QReadLocker lock(&lock_);
    auto node = entity_factory->GetNode(ID);
    if(node){
        return node->getTypes();
    }
    return {};
}


void ModelController::setProjectSaved(QString path)
{
    setProjectModified(false);
    if(path != ""){
        //Update the file save path.
        setProjectPath(path);
    }
}



bool ModelController::storeEntity(Entity* item, int desired_id)
{
    bool success = false;
    if(item){
        try{
            entity_factory->RegisterEntity(item, desired_id);
        }catch(const std::exception & ex){
            qCritical() << ex.what();
            return false;
        }
        auto id = item->getID();

        bool inserted = false;
        if(item->isNode()){
            if(!node_ids_.contains(id)){
                node_ids_.insert(id);
                auto node = (Node*) item;
                connect(node, &Node::acceptedEdgeKindsChanged, [=](){emit NodeEdgeKindsChanged(id);});
                connect(node, &Node::typesChanged, [=](){emit NodeTypesChanged(id);});
                
                emit NodeConstructed(node->getParentNodeID(), id, node->getNodeKind());
                inserted = true;
            }
        }else if(item->isEdge()){
            if(!edge_ids_.contains(id)){
                edge_ids_.insert(id);
                auto edge = (Edge*) item;
                emit EdgeConstructed(id, edge->getEdgeKind(), edge->getSourceID(), edge->getDestinationID());
                inserted = true;
            }
        }

        if(inserted){
            
            connect(item, &Entity::dataChanged, [=](int ID, QString key_name, QVariant value, bool is_protected){
                DataUpdate data;
                data.key_name = key_name;
                data.value = value;
                data.is_protected = is_protected;

                emit DataChanged(ID, data);
            });
            
            connect(item, &Entity::dataRemoved, this, &ModelController::DataRemoved, Qt::UniqueConnection);
        }
        success = inserted;
    }
    return success;
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


bool ModelController::attachChildNode(Node *parent_node, Node *node, bool notify_view)
{
    if(parent_node->addChild(node)){
        if(notify_view){
            storeNode(node);
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


//Look inside target_node for things which could become a dependant of the the definition
QList<Node*> ModelController::get_matching_dependant_of_definition(Node* target_node, Node* definition){
    QList<QSet<NODE_KIND> > dependant_kind_list;
    
    if(target_node->isInstance()){
        dependant_kind_list.append(definition->getInstanceKinds());
    }else{
        dependant_kind_list.append(definition->getImplKinds());
        dependant_kind_list.append(definition->getInstanceKinds());
    }
    
    QList<Node*> matching_nodes;
    
    for(auto &dependant_kinds : dependant_kind_list){
        //For each child in parent, check to see if any Nodes match Label/Type
        for(auto target_child : target_node->getChildrenOfKinds(dependant_kinds, 0)){
            if(target_child->getDefinition() == definition){
                matching_nodes << target_child;
            }else if(target_child->getDefinition()){
                
            }else{
                auto target_type_data = target_child->getData("type");
                auto definition_type_data = definition->getData("type");

                if(Data::CompareData(target_type_data, definition_type_data)){
                    matching_nodes << target_child;
                }
            }
        }
    }

    return matching_nodes;
}

void ModelController::destructEdge_(Edge *edge){
    if(edge){
        auto id = edge->getID();
        //Teardown specific edge classes.
        auto edge_kind = edge->getEdgeKind();
        auto src = edge->getSource();
        auto dst = edge->getDestination();
        
        if(src && dst){
            switch(edge_kind){
            case EDGE_KIND::DEFINITION:{
                setupDefinitionRelationship2(src, dst, false);
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
    return destructEntities({item});
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
        for(auto edge : node->getEdges()){
            edges.insert(edge);
        }
    }

    if(!edges.empty()){
        //Create an undo state which groups all edges together
        auto action = getNewAction(GRAPHML_KIND::EDGE);
        action.Action.type = ACTION_TYPE::DESTRUCTED;
        action.xml = exportGraphML(edges.toList(), true);

        //qCritical() << "EDGE BACKUP: " << action.xml;
        addActionToStack(action);

        //Destruct all the edges
        for(auto e : edges){
            destructEdge_((Edge*)e);
        }
    }

    for(auto entity : sorted_nodes){
        auto node = (Node*) entity;
        //qCritical() << "Exporting: " << node->toString();
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
        auto entity = entity_factory->GetEntity(action.entity_id);

        switch(action.Action.kind){
        case GRAPHML_KIND::NODE:
        case GRAPHML_KIND::EDGE:{
            if(!entity){
                qCritical() << " NO ENTITY WITH ID: " << action.entity_id  << "INSIDE: " << action.parent_id;
                qCritical() << action.xml;
            }else{
                //qCritical() << "Destructing Entity: " << entity->toString();
            }

            return destructEntities({entity});
        }
        case GRAPHML_KIND::DATA:{
            return destructData_(entity, action.Data.key_name);
        }
        default:
            break;
        }
        break;
    }
    case ACTION_TYPE::DESTRUCTED:{
        switch(action.Action.kind){
        case GRAPHML_KIND::EDGE:
        case GRAPHML_KIND::NODE:{
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
            qCritical() << "Entity ID: " << action.entity_id;
            qCritical() << "Kind: " << getGraphMLKindString(action.Action.kind);
            qCritical() << "Type: " << getActionTypeString(action.Action.type);
            
            if(action.Action.kind == GRAPHML_KIND::DATA){
                qCritical() << "action.Data.key_name:" << action.Data.key_name;
                qCritical() << "action.Data.old_value:" << action.Data.old_value;
                qCritical() << "action.Data.new_value:" << action.Data.new_value;   
            }
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

        if(node->isImplicitlyConstructed()){
            return false;
        }

        auto node_kind = node->getNodeKind();
        auto parent_node = node->getParentNode();
        auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;

        switch(node_kind){
            case NODE_KIND::ATTRIBUTE_INSTANCE:
                if(parent_node_kind == NODE_KIND::PERIODICEVENT){
                    return false;
                }
                break;
            case NODE_KIND::INPUT_PARAMETER:
            case NODE_KIND::RETURN_PARAMETER:
                //return false;
            default:
                break;
        }

        if(node->getDefinition()){
            switch(node_kind){
            case NODE_KIND::CLASS_INSTANCE:{
                auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;
                 switch(parent_node_kind){
                    case NODE_KIND::COMPONENT_INSTANCE:
                        return false;
                    default:
                        break;
                }
                break;
            }
                
            case NODE_KIND::FUNCTION_CALL:
            case NODE_KIND::SERVER_REQUEST:
            case NODE_KIND::OUTEVENTPORT_IMPL:
            case NODE_KIND::SERVER_PORT:
            case NODE_KIND::CLIENT_PORT:
            case NODE_KIND::COMPONENT_INSTANCE:
                // These node kinds can be destroyed at any time
                break;
            case NODE_KIND::AGGREGATE_INSTANCE:{
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


QList<EDGE_KIND> ModelController::getPotentialEdgeClasses(Node *src, Node *dst)
{
    QList<EDGE_KIND> edge_kinds;

    for(auto edge_kind : GetEdgeOrderIndexes()){
        if(src->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::SOURCE) && dst->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::TARGET)){
            edge_kinds << edge_kind;
        }
    }
    return edge_kinds;
}

QString ModelController::_copy(QList<Entity *> selection)
{
    return exportGraphML(selection, false);
}


bool ModelController::storeNode(Node* node, int desired_id, bool store_children, bool add_action)
{
    bool success = false;
    if(node){
        success = storeEntity(node, desired_id);

        if(success){
            if(store_children){
                //Get all our children
                for(auto child : node->getChildren()){
                    if(child->isImplicitlyConstructed()){
                        storeEntity(child, -1);
                    }
                }
                
                for(auto edge : node->getEdges()){
                    if(edge->isImplicitlyConstructed()){
                        storeEntity(edge, -1);
                    }
                }
            }

            if(add_action){
                //Construct an ActionItem to reverse Node Construction.
                auto action = getNewAction(GRAPHML_KIND::NODE);
                action.Action.type = ACTION_TYPE::CONSTRUCTED;
                action.entity_id = node->getID();
                action.parent_id = node->getParentNodeID();
                action.xml = node->toGraphML();
                addActionToStack(action);
            }
        }else{
            //qCritical() << "Trying to store: " << node->toString() << " @ ID: " << desired_id;
        }
    }
    return success;
}

void ModelController::setupModel()
{
    set_persistent_node(construct_node(0, NODE_KIND::MODEL));
    if(model){
        for(auto child : model->getChildren()){
            set_persistent_node(child);
        }
        
        auto label_data = model->getData("label");
        if(label_data){
            connect(label_data, &Data::dataChanged, [=](QVariant label){
                emit ProjectNameChanged(label.toString());
            });
        }

        storeNode(model, -1, true, false);
    }
}


bool ModelController::setupDefinitionRelationship2(Node* instance, Node* definition, bool setup){
    if(!(instance && definition)){
        return false;
    }
    

    auto construct_instance = instance->isInstance();
    auto construct_implementation = instance->isImpl();


    //Construct the Relationship at the Node Level
    if(instance->gotEdgeTo(definition, EDGE_KIND::DEFINITION)){
        if(construct_instance){
            if(setup){
                definition->addInstance(instance);
            }else{
                definition->removeInstance(instance);
            }
        }else if(construct_implementation){
            if(setup){
                definition->addImplementation(instance);
            }else{
                definition->removeImplementation(instance);
            }
        }
    }

    if(setup && isUserAction()){
        //Get our direct children of definitions
        for(auto def_child : instance->getRequiredInstanceDefinitions()){
            bool construct_dependant = true;


            for(auto matching_dependant : get_matching_dependant_of_definition(instance, def_child)){
                construct_edge(EDGE_KIND::DEFINITION, matching_dependant, def_child);

                if(matching_dependant->getDefinition() == def_child){
                    construct_dependant = false;
                    break;
                }
            }

            if(construct_dependant){
                QList<QSet<NODE_KIND> > dependant_kind_list;

                if(construct_instance){
                    dependant_kind_list.append(def_child->getInstanceKinds());
                }else{
                    dependant_kind_list.append(def_child->getImplKinds());
                    dependant_kind_list.append(def_child->getInstanceKinds());
                }

                
                for(auto &dependant_kinds : dependant_kind_list){
                    bool got_node = false;    
                    //Try Either
                    for(auto kind : dependant_kinds){
                        auto dependant_child = construct_connected_node(instance, kind, def_child, EDGE_KIND::DEFINITION);
                        if(dependant_child){
                            got_node = true;
                            break;
                        }
                    }
                    if(got_node){
                        break;
                    }
                }
            }
        }
    }
    
    return true;
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


        bool construct_instance = setup && isUserAction() && !eventport->IsEdgeRuleActive(Node::EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);
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

        if(eventport->getNodeKind() == NODE_KIND::EXTERNAL_ASSEMBLY){
            //Connect our children
            for(auto child : eventport->getChildren()){
                construct_edge(EDGE_KIND::AGGREGATE, child, dst);
            }
        }

        //Check for an edge between the EventPort and the Aggregate
        if(eventport->gotEdgeTo(aggregate, EDGE_KIND::AGGREGATE)){
            if(setup){
                //Edge Created Set Aggregate relation.
                eventport->setAggregate(aggregate);
            }else{
                //Unset the Aggregate
                eventport->unsetAggregate();
            }
            return true;
        }
    }
    return false;
}

bool ModelController::linkData_(Node* src, QString src_key, Node* dst, QString dst_key, bool setup_link){
    if(src && dst){
        auto src_data = src->getData(src_key);
        auto dst_data = dst->getData(dst_key);
        if(src_data && dst_data){
            return src_data->linkData(dst_data, setup_link);
        }
    }
    return false;
}

bool ModelController::setupDataRelationship(Node* src, Node* dst, bool setup)
{
    DataNode::BindDataRelationship(src, dst, setup);
    return true;
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

bool ModelController::storeEdge(Edge *edge, int desired_id)
{
    bool success = false;
    if(edge){
        //Used to be Before
        success = storeEntity(edge, desired_id);

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
            setupDefinitionRelationship2(src, dst, true);
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
        
    }
    return success;
}

QString ModelController::getProjectAsGraphML(bool functional_export)
{
    lock_.lockForRead();
    QString data = exportGraphML(QList<Entity*>{model}, true, functional_export);
    lock_.unlock();
    return data;
}



QSet<EDGE_KIND> ModelController::getCurrentEdgeKinds(QList<int> ids)
{
    QReadLocker lock(&lock_);
    QSet<EDGE_KIND> edge_kinds;

    for(auto entity : getUnorderedEntities(ids)){
        if(entity->isNode()){
            auto node = (Node*) entity;
            for(auto edge : node->getEdges(0)){
                edge_kinds.insert(edge->getEdgeKind());
            }
        }
    }
    return edge_kinds;
}


QSet<NODE_KIND> ModelController::getValidNodeKinds(int ID)
{
    
    QWriteLocker lock(&lock_);
    QSet<NODE_KIND> node_kinds;

    auto parent_node = entity_factory->GetNode(ID);

    if(parent_node){
        for(auto node_kind : parent_node->getUserConstructableNodeKinds()){
            auto temp_node = entity_factory->CreateTempNode(node_kind);
            if(temp_node){
                if(parent_node->canAdoptChild(temp_node)){
                    node_kinds << node_kind;
                }
                entity_factory->DestructEntity(temp_node);
            }
        }
    }
    return node_kinds;
}

QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > ModelController::getValidEdgeKinds(QList<int> ids){
    QReadLocker lock(&lock_);
    QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > valid_pairs;

    auto entities = getUnorderedEntities(ids);

    if(!entities.isEmpty()){
        valid_pairs.first = entity_factory->getEdgeKinds().toSet();
        valid_pairs.second = entity_factory->getEdgeKinds().toSet();
    }

    for(auto entity : entities){
        if(entity->isNode()){
            auto node = (Node*) entity;
            valid_pairs.first &= node->getAcceptedEdgeKind(EDGE_DIRECTION::SOURCE);
            valid_pairs.second &= node->getAcceptedEdgeKind(EDGE_DIRECTION::TARGET);
        }else{
            valid_pairs.first.clear();
            valid_pairs.second.clear();
        }
        if(valid_pairs.first.isEmpty() && valid_pairs.second.isEmpty()){
            break;
        }
    }
    return valid_pairs;
}


QList<int> ModelController::getConstructablesConnectableNodes(int constructable_parent_id, NODE_KIND constructable_node_kind, EDGE_KIND connection_edge_kind){
    QWriteLocker lock(&lock_);
    
    QList<Node*> nodes;

    auto parent_node = entity_factory->GetNode(constructable_parent_id);
    if(parent_node){
        auto child_node = entity_factory->CreateTempNode(constructable_node_kind);
        if(child_node){
            if(parent_node->addChild(child_node)){
                nodes = _getConnectableNodes({child_node}, connection_edge_kind).values(EDGE_DIRECTION::TARGET);
            }
            entity_factory->DestructEntity(child_node);
        }
    }
    
    return getIDs(nodes);
}


QSet<NODE_KIND> ModelController::getGUINodeKinds(){
    auto node_set = QSet<NODE_KIND>::fromList(entity_factory->getNodeKinds());
    node_set.remove(NODE_KIND::NONE);
    node_set.remove(NODE_KIND::IDL);
    return node_set;
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
        //qCritical() << "== REQUEST: " << parentID << " KEY: " << keyName << " = " << dataValue;
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
bool ModelController::importProjects(QStringList xml_list)
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
                QString title = "Cannot import project";
                emit Notification(MODEL_SEVERITY::ERROR, title);
                success = false;
            }
        }
        unsetModelAction(MODEL_ACTION::IMPORT);
    }
    emit ShowProgress(false);
    emit ActionFinished();
    return success;
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

double ModelController::compare_version(QString current_version, QString compare_version){
    

    auto compare_first =  compare_version.indexOf(".") + 1;
    auto compare_second =  compare_version.indexOf(".", compare_first);

    auto current_first =  current_version.indexOf(".") + 1;;
    auto current_second =  current_version.indexOf(".", current_first);

    bool current_okay = false;
    bool compare_okay = false;
    //Only compare Major.Minor
    auto current_v = current_version.left(current_second).toDouble(&current_okay);
    auto compare_v = compare_version.left(compare_second).toDouble(&compare_okay);

    if(!current_v){
        //Should never happen
        qCritical() << "compare_version(): Can't tokenize version number: " << current_version;
    }
    if(!compare_okay){
        compare_v = 0;
    }

    return compare_v - current_v;
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
        QString title = "Cannot import graphml";
        QString description = "importGraphML(): Document is not valid XML";
        emit Notification(MODEL_SEVERITY::ERROR, title, description);
        return false;
    }

    int error_count = 0;

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);

    auto current_entity = new TempEntity(GRAPHML_KIND::NODE);
    current_entity->setID(parent->getID());

    //State flags
    const bool IS_OPEN = isModelAction(MODEL_ACTION::OPEN);
    const bool IS_IMPORT = isModelAction(MODEL_ACTION::IMPORT);
    const bool IS_UNDO_REDO = isModelAction(MODEL_ACTION::UNDO) || isModelAction(MODEL_ACTION::REDO);
    const bool IS_PASTE = isModelAction(MODEL_ACTION::PASTE);
    const bool IS_PARTIAL_IMPORT = IS_UNDO_REDO || IS_PASTE;
    const bool UPDATE_PROGRESS = !isModelAction(MODEL_ACTION::NONE);
    const bool GENERATE_UUID_FROM_ID = IS_IMPORT && (parent == workerDefinitions);

    if(UPDATE_PROGRESS){
        ProgressUpdated_("Parsing Project");
        ProgressChanged_(-1);
    }

    while(xml.readNext() != QXmlStreamReader::EndDocument){
        auto kind = getGraphMLKindFromString(xml.name());

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
                            //First time we've seen it
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
                            auto model_version = compare_version(APP_VERSION(), value);

                            if(model_version > 0){
                                QString title = "Loading model from future MEDEA";
                                QString description = "Model was created in a new version of MEDEA (v" % value % "). Some functionality may not be supported.";
                                emit Notification(MODEL_SEVERITY::WARNING, title, description);
                            }else if(model_version < 0){
                                QString title = "Loading model from legacy MEDEA";
                                QString description = "Model was created in MEDEA (v" % value % "). Some functionality may have changed. Model version updated to: " % APP_VERSION();
                                emit Notification(MODEL_SEVERITY::INFO, title, description);
                                //Update
                                value = APP_VERSION();
                            }else{
                                value = APP_VERSION();
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

                    if(GENERATE_UUID_FROM_ID){
                        //If we are meant to generate UUIDS
                        //Use the String'd ID as the seed for the UUID
                        auto uuid_id = ExportIDKey::GetUUIDOfValue(id);
                        entity->addData("uuid", uuid_id);
                        unique_entity_ids.push_back(id);
                    }

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
        if(!entity){
            qCritical() << "ImportGraphML: Cannot process UUID'd entity with id: '" << id << "'";
            continue;
        }

        bool handle_uuid = IS_IMPORT;
        auto kind = entity->getDataValue("kind").toString();

        if(!handle_uuid && entity->isNode()){
            auto parent_node_kinds_set = entity->getParentStack().toSet();
            const QSet<NODE_KIND> always_handle_kinds = {NODE_KIND::HARDWARE_DEFINITIONS, NODE_KIND::WORKER_DEFINITIONS};
            handle_uuid = parent_node_kinds_set.intersects(always_handle_kinds);
        }

        //Handle UUIDS
        if(handle_uuid){
            
            const auto& uuid = entity->getDataValue("uuid").toString();
            //If we have a uuid, we should set the entity as read-only
            entity->addData("readOnly", true);
            entity->setUUIDMatched(true);
            
            //Lookup the entity in the 
            auto matched_entity = entity_factory->GetEntityByUUID(uuid);
            if(matched_entity && matched_entity->isNode()){
                auto matched_node = (Node*) matched_entity;
                //Produce a notification for updating shared_datatypes
                
                if(matched_node->getNodeKind() == NODE_KIND::SHARED_DATATYPES || matched_node->getNodeKind() == NODE_KIND::CLASS){
                    auto version = entity->getDataValue("version").toString();
                    auto old_version = matched_entity->getDataValue("version").toString();
                    auto old_label = matched_entity->getDataValue("label").toString();
                    auto node_kind = matched_node->getDataValue("kind").toString();

                    if(!version.isEmpty() && !old_version.isEmpty()){
                        auto version_compare = compare_version(old_version, version);

                        if(version_compare > 0){
                            QString title = "Model contains a newer " + node_kind + " named '" + old_label + "'";
                            QString description = "Updated from '" % old_version % "' to '" % version % "'. Please check usage.";
                            emit Notification(MODEL_SEVERITY::WARNING, title, description);
                        }else if(version_compare < 0){
                            QString title = "Model contains an older " + node_kind + " named '" + old_label + "'";
                            QString description = "Reverted from '" % old_version % "' to '" % version % "'. Please check usage.";
                            emit Notification(MODEL_SEVERITY::WARNING, title, description);
                        }else{
                        }
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

    if(UPDATE_PROGRESS){
        ProgressUpdated_("Constructing Nodes");
    }
    QQueue<Node*> implicitly_constructed_nodes;

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
        if(IS_PASTE && parent_entity == current_entity){
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
            if(parent_entity->GotImplicitlyConstructedNodeID(kind)){
                //Get the ID of the next auto constructed ID
                auto id = parent_entity->TakeNextImplicitlyConstructedNodeID(kind);
                node = entity_factory->GetNode(id);
            }else{
                node = construct_node(parent_node, kind);
            }
        }

        if(node){
            //Check to see if the node constructed some other stuff, It shouldn 't have any children.
            for(auto child : node->getChildren()){
                if(child->isImplicitlyConstructed()){
                    entity->AddImplicitlyConstructedNodeID(child->getNodeKind(), child->getID());
                    if(!implicitly_constructed_nodes.contains(child)){
                        implicitly_constructed_nodes << child;
                    }
                }
            }

            for(auto edge : node->getEdges()){
                if(edge->isImplicitlyConstructed()){
                    entity->AddImplicitlyConstructedEdgeID(edge->getEdgeKind(), edge->getID());
                }
            }

            //If we have matched something which is already got a parent node we shouldn't overwrite its data
            //This will stop any data stored in the aspects getting overwritten
            if(node->getID() > 0 && !IS_OPEN){
                if(!entity->isUUIDMatched()){
                    //qCritical() << node->toString() << " IGNORING IMPORTED DATA";
                    entity->clearData();
                }
            }


            bool is_model = kind == NODE_KIND::MODEL;

            bool got_parent = node->getParentNode();
            bool requires_parenting = !got_parent && !is_model;
            bool implicitly_created = node->isImplicitlyConstructed();
            bool need_to_store = false;
            
            if(requires_parenting || implicitly_created){
                //If the node is not attached to a parent, or it was implicilty constructed, set the data ahead of time
                for(auto key_name : entity->getKeys()){
                    auto value = entity->getDataValue(key_name);
                    //TODO: WORK OUT IF WE SHOULD ALLOW DATA WE DON'T IMPICILTLY CREATE
                    setData_(node, key_name, value, false);
                }
                //Remove the data as we have already attached it
                entity->clearData();
            }

            if(requires_parenting){
                //If it's not got a parent, set it.
                got_parent = attachChildNode(parent_node, node, false);
                need_to_store = true;
            }

            if(need_to_store || implicitly_created){
                auto add_action_state = !implicitly_created;
                storeNode(node, entity->getPreviousID(), false, add_action_state);
            }

            for(auto key_name : entity->getKeys()){
                auto value = entity->getDataValue(key_name);
                setData_(node, key_name, value, true);
            }

            if(got_parent || is_model){
                entity->setID(node->getID());
            }else{
                //Destroy!
                entity_factory->DestructEntity(node);
                node = 0;
            }

            
        }
        if(!node){
            QString title = "Cannot create Node '" + entity->getKind() + "'";
            QString description = "importGraphML(): Document line #" % QString::number(entity->getLineNumber());
            error_count ++;
            emit Notification(MODEL_SEVERITY::ERROR, title, description);
        }

        if(UPDATE_PROGRESS){
            ProgressChanged_(++entities_made / entities_total_perc);
        }
    }

    while(implicitly_constructed_nodes.size()){
        auto node = implicitly_constructed_nodes.dequeue();
        storeNode(node, -1, false, false);
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
        }else if(IS_PARTIAL_IMPORT){
            //Try and see if the integer version of the ID stored in the GraphML Still exists in the model.
            src_node = entity_factory->GetNode(entity->getSourceIDInt());
        }

        if(dst_entity && dst_entity->gotID()){
            dst_node = entity_factory->GetNode(dst_entity->getID());
        }else if(IS_PARTIAL_IMPORT){
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
                QString title = "Cannot create edge";
                QString description = "importGraphML(): Document line # " % QString::number(entity->getLineNumber()) % "\nNo valid edge kinds";
                error_count ++;
                emit Notification(MODEL_SEVERITY::ERROR, title, description);
            }
        }else{
            QString title = "Cannot create edge";
            QString description = "importGraphML(): Document line # " % QString::number(entity->getLineNumber()) % "\nMissing Source or destination";
            error_count ++;
            emit Notification(MODEL_SEVERITY::ERROR, title, description);
        }
    }

    if(UPDATE_PROGRESS){
        ProgressUpdated_("Constructing Edges");
    }

    int edge_iterations = 0;
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
                bool need_to_store = false;
                //Check if we have an edge first
                edge = src_node->getEdgeTo(dst_node, edge_kind);

                if(!edge){
                    edge = construct_edge(edge_kind, src_node, dst_node, false);
                    need_to_store = true;
                }

                if(edge){
                    if(need_to_store){
                        storeEdge(edge, entity->getPreviousID());
                    }

                    for(auto key_name : entity->getKeys()){
                        auto value = entity->getDataValue(key_name);
                        setData_(edge, key_name, value, true);
                    }

                    //Point the tempEntity to the newly constructed entity
                    entity->setID(edge->getID());
                }
            }else{
                QString title = "Cannot create edge";
                QString description = "importGraphML(): Document line # " % QString::number(entity->getLineNumber()) % "\nInternal Error";
                error_count ++;
                emit Notification(MODEL_SEVERITY::ERROR, title, description);
            }

            if(edge){
                if(UPDATE_PROGRESS){
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
        edge_iterations ++;
    }

    if(UPDATE_PROGRESS){
        ProgressChanged_(100);
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
    
    auto unordered_items = getUnorderedEntities(ids);
    //Got a list of entites
    auto ordered_items = getOrderedEntities(unordered_items);

    if(canCut(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CUT);
    }

    if(canCopy(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_COPY);
    }

    if(canRemove(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_REMOVE);
    }

    if(canPaste(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_PASTE);
    }

    if(canReplicate(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_REPLICATE);
    }

    if(canChangeIndex(unordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CHANGE_INDEX);
    }

    if(canChangeRow(unordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CHANGE_ROW);
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

            if(gotImplementation(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_IMPLEMENTATION);
            }

            if(gotInstances(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_INSTANCES);
            }

            if(node->getEdgeCount() > 0){
                properties.insert(SELECTION_PROPERTIES::GOT_EDGES);
            }
        }
    }

    return properties;
}

bool ModelController::canCut(const QList<Entity *>& ordered_selection)
{
    if(canRemove(ordered_selection)){
        for(const auto& item : ordered_selection){
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



bool ModelController::canCopy(const QList<Entity *>& ordered_selection)
{
    bool valid = !ordered_selection.isEmpty();
    Node* parent = 0;

    for(const auto& item : ordered_selection){
        if(item->isNode()){
            Node* node = (Node*) item;
            if(!canDeleteNode(node)){
                return false;
            }

            auto current_parent = node->getParentNode();

            //Dont allow copy from any definitions, except 
            if(node->isDefinition()){
                switch(node->getNodeKind()){
                    case NODE_KIND::ENUM:
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

bool ModelController::canPaste(const QList<Entity *>& ordered_selection)
{
    if(ordered_selection.size() == 1){
        Entity* item = ordered_selection.first();

        if(item && item->isNode()){
            auto node = (Node*) item;

            if(!node->isReadOnly() && !node->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER)){
                if(node->isInstanceImpl() ){
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

bool ModelController::canChangeIndex(const QList<Entity *>& unordered_selection)
{
    bool valid = unordered_selection.count() == 1;

    if(valid){
        for(const auto& entity : unordered_selection){
            auto data = entity->getData("index");
            if(!data || data->isProtected()){
                valid = false;
                break;
            }
        }
    }
    return valid;
}

bool ModelController::canChangeRow(const QList<Entity *>& unordered_selection)
{
    bool valid = unordered_selection.count() == 1;

    if(valid){
        for(const auto& entity : unordered_selection){
            auto data = entity->getData("row");
            if(!data || data->isProtected()){
                valid = false;
                break;
            }
        }
    }
    return valid;
}

bool ModelController::canReplicate(const QList<Entity *>& ordered_selection)
{
    if(canCopy(ordered_selection)){
        //Get the first item, check if we can paste into it
        return canPaste({ordered_selection.first()});
    }
    return false;
}

bool ModelController::canRemove(const QList<Entity *>& ordered_selection)
{
    if(ordered_selection.isEmpty()){
        return false;
    }

    for(const auto& entity : ordered_selection){
        if(entity->isNode()){
            auto node = (Node*) entity;
            auto parent_node = node->getParentNode();

            if(!canDeleteNode(node)){
                return false;
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

bool ModelController::gotImplementation(Node* node){
    auto definition = node ? node->getDefinition(true) : 0;
    return definition ? definition->getImplementations().count() > 0 : false;
}

Node* ModelController::getDefinition(Node* node){
    if(node){
        return node->getDefinition(true);
    }
    return 0;
}

bool ModelController::gotInstances(Node* node){
    auto definition = node ? node->getDefinition(true) : 0;
    return definition ? definition->getInstances().count() > 0 : false;
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
    auto definition = getDefinition(entity_factory->GetNode(id));

    if(definition){
        auto impls = definition->getImplementations();
        if(impls.size()){
            return (*impls.begin())->getID();
        }
    }
    return -1;
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