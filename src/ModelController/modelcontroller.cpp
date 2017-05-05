#include "modelcontroller.h"
#include <QDebug>
#include <algorithm>
#include <QDateTime>
#include <QEventLoop>
#include <QSysInfo>
#include <QDir>
#include <QStringBuilder>
#include <QNetworkInterface>
#include <QThread>
#include <QFile>

#include "entityfactory.h"
#include "tempentity.h"

#include "Entities/edge.h"
#include "Entities/data.h"
#include "Entities/node.h"
#include "Entities/key.h"

#include "Entities/InterfaceDefinitions/eventport.h"
#include "Entities/InterfaceDefinitions/aggregate.h"
#include "Entities/InterfaceDefinitions/datanode.h"
#include "Entities/BehaviourDefinitions/workerprocess.h"
#include "Entities/DeploymentDefinitions/eventportdelegate.h"



int get_int_from_qstring(QString ID, int default_value = -1){
    bool okay;
    auto val = ID.toInt(&okay);
    if(okay){
        return val;
    }else{
        return default_value;
    }
}


bool UNDO = true;
bool REDO = false;
bool SETUP_AS_INSTANCE = true;
bool SETUP_AS_IMPL = false;

ModelController::ModelController() :QObject(0)
{

    controllerThread = new QThread();
    moveToThread(controllerThread);

    connect(this, &ModelController::initiateTeardown, this, &QObject::deleteLater, Qt::QueuedConnection);
    connect(this, &ModelController::initiateTeardown, controllerThread, &QThread::quit, Qt::QueuedConnection);
    controllerThread->start();

    entity_factory = EntityFactory::getNewFactory();


    qRegisterMetaType<GRAPHML_KIND>("GRAPHML_KIND");
    qRegisterMetaType<EDGE_KIND>("EDGE_KIND");
    qRegisterMetaType<NODE_KIND>("NODE_KIND");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<ReadOnlyState>("ReadOnlyState");

    //Model::resetID();



    PASTE_USED = false;
    REPLICATE_USED = false;
    IMPORTING_PROJECT = false;
    UNDOING = false;
    OPENING_PROJECT = false;
    IMPORTING_WORKERDEFINITIONS = false;
    INITIALIZING = true;
    REDOING = false;
    DESTRUCTING_CONTROLLER = false;
    CUT_USED = false;
    IMPORTING_SNIPPET = false;
    model = 0;
    //Construct
    behaviourDefinitions = 0;
    interfaceDefinitions = 0;
    deploymentDefinitions = 0;
    hardwareDefinitions = 0;
    assemblyDefinitions = 0;
    workerDefinitions = 0;

    projectDirty = false;

    currentActionID = 0;
    currentActionItemID = 0;
    actionCount = 0;
    currentAction = "";


    
    nonSnippetableKinds << NODE_KIND::OUTEVENTPORT_IMPL << NODE_KIND::INEVENTPORT_IMPL;
}
/*
void ModelController::connectViewController(ViewController *view)
{
    
}

void ModelController::disconnectViewController(ViewController *view)
{

    view->disconnect(this);
    this->disconnect(view);
    emit initiateTeardown();
}
*/



void ModelController::setupController()
{
    QWriteLocker lock(&lock_);
    setupModel();
    loadWorkerDefinitions();
    clearHistory();
    INITIALIZING = false;

    emit controller_ProjectFileChanged("Untitled Project");
    emit projectModified(true);
    emit controller_IsModelReady(true);
    emit controller_ActionFinished();
}

ModelController::~ModelController()
{
    DESTRUCTING_CONTROLLER = true;

    destructNode(workerDefinitions);
    destructNode(model); 
}

/**
 * @brief NewController::loadWorkerDefinitions Loads in both the compiled in WorkerDefinitions and Externally defined worker defintions.
 */
void ModelController::loadWorkerDefinitions()
{
    if(workerDefinitions){
        QList<QDir> workerDirectories;
        workerDirectories << QDir(":/WorkerDefinitions");
        if(externalWorkerDefPath != ""){
            workerDirectories << QDir(externalWorkerDefPath);
        }


        QStringList filesToLoad;
        QStringList fileExtension("*.worker");

        foreach(QDir directory, workerDirectories){
            //Foreach *.worker.graphml file in the workerDefPath, load the graphml.
            foreach(QString fileName, directory.entryList(fileExtension)){
                QString importFileName = directory.absolutePath() + "/" + fileName;
                filesToLoad << importFileName;
            }
        }

        IMPORTING_WORKERDEFINITIONS = true;
        foreach(QString file, filesToLoad){
            QPair<bool, QString> data = readFile(file);
            //If the file was read.
            if(data.first){
                bool success = _newImportGraphML(data.second, workerDefinitions);

                if(!success){
                    QString message = "Cannot import worker definition '" + file +"'";
                    //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_WARNING);
                }else{
                    //qCritical() << "Loaded Worker Definition: " << file;
                }
            }else{
                QString message = "Cannot read worker definition '" + file +"'";
                //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_WARNING);
            }
        }
        IMPORTING_WORKERDEFINITIONS = false;
    }
    //Once we have loaded in workers, we should keep a dictionary lookup for them.
}

QString ModelController::_exportGraphMLDocument(QList<int> entityIDs, bool allEdges, bool GUI_USED, bool ignoreVisuals)
{
    Q_UNUSED(GUI_USED);
    bool exportAllEdges = allEdges;

    QString keyXML, edgeXML, nodeXML;
    QList<Key*> containedKeys;

    QList<Entity*> containedEntities;
    QList<Node*> topLevelNodes;
    QList<Entity*> topLevelEntities;
    QList<Edge*> containedEdges;


    foreach(Entity* entity, getOrderedSelection(entityIDs)){
        foreach(Key* key, entity->getKeys()){
            if(!containedKeys.contains(key)){
                containedKeys.append(key);
            }
        }

        Node* node = (Node*) entity;
        Edge* edge = (Edge*) entity;

        if(topLevelEntities.contains(entity)){
            topLevelEntities += entity;
        }

        if(entity->isNode()){
            if(!topLevelNodes.contains(node)){
                topLevelNodes.append(node);
            }
            foreach(Node* child, node->getChildren()){
                if(!containedEntities.contains(child)){
                    containedEntities.append(child);
                }
            }
            foreach(Edge* childEdge, node->getAllEdges()){
                if(!containedEntities.contains(childEdge)){
                    containedEdges.append(childEdge);
                }
                if(!containedEdges.contains(childEdge)){
                    containedEdges.append(childEdge);
                }
            }
        }else{
            if(!containedEdges.contains(edge)){
                containedEdges.append(edge);
            }
        }
    }

    foreach(Entity* entity, containedEntities){
        foreach(Key* key, entity->getKeys()){
            if(!containedKeys.contains(key)){
                containedKeys.append(key);
            }
        }
    }

    //Export the XML for this node
    foreach(Key* key, containedKeys){
        keyXML += key->toGraphML(1);
    }

    foreach(Node* node, topLevelNodes){
        if(!ignoreVisuals){
            nodeXML += node->toGraphML(2);
        }else{
            nodeXML += node->toGraphMLNoVisualData(2);
        }
    }

    foreach(Edge* edge, containedEdges){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        bool exportEdge = false;
        if(exportAllEdges || (containedEntities.contains(src) && containedEntities.contains(dst))){
            exportEdge = true;
        }else{
            switch(edge->getEdgeKind()){
                case EDGE_KIND::AGGREGATE:
                case EDGE_KIND::ASSEMBLY:
                case EDGE_KIND::DEPLOYMENT:
                case EDGE_KIND::DEFINITION:{
                    exportEdge = true;
                    break;
                }
                default:
                    break;
            }
        }


        if(exportEdge){
            //Export the XML for this node
            edgeXML += edge->toGraphML(2);
        }
    }

    QString returnable = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    returnable +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";
    returnable += keyXML;
    returnable +="\n\t<graph edgedefault=\"directed\" id=\"parentGraph0\">\n";
    returnable += nodeXML;
    returnable += edgeXML;
    returnable += "\t</graph>\n";
    returnable += "</graphml>";

    return returnable;
}

QString ModelController::_exportGraphMLDocument(Node *node, bool allEdges, bool GUI_USED)
{
    QList<int> nodeIDs;
    nodeIDs << node->getID();
    return _exportGraphMLDocument(nodeIDs, allEdges, GUI_USED);
}


bool ModelController::_clear()
{
    bool reply = askQuestion("Clear Model?", "Are you sure you want to clear the model? You cannot undo this action.");
    if(reply){
        triggerAction("Clearing Model");

        emit controller_ActionProgressChanged(0,"Clearing model");
        QList<Node*> childNodes = interfaceDefinitions->getChildren(0);
        // while(!childNodes.isEmpty())
        for(int i=0; i < childNodes.size(); i++){
            Node* child = childNodes[i];
            destructNode(child);
        }
        childNodes.clear();

        childNodes = behaviourDefinitions->getChildren(0);
        for(int i=0; i < childNodes.size(); i++){
            Node* child = childNodes[i];
            destructNode(child);
        }
        childNodes.clear();

        childNodes = hardwareDefinitions->getChildren(0);
        for(int i=0; i < childNodes.size(); i++){
            Node* child = childNodes[i];

            // don't delete Localhost Nodes
            if (child->getDataValue("kind") != "HardwareNode") {
                destructNode(child);
            }
        }
        childNodes.clear();
        childNodes = assemblyDefinitions->getChildren(0);
        for(int i=0; i < childNodes.size(); i++){
            Node* child = childNodes[i];

            // don't delete ManagementComponents
            if (child->getDataValue("kind") != "ManagementComponent") {
                destructNode(child);
            }
        }
        childNodes.clear();

        clearHistory();

        emit controller_ActionProgressChanged(100);
        return true;
    }
    return reply;
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


void ModelController::_setData(Entity *parent, QString keyName, QVariant dataValue, bool addAction)
{
    if(DESTRUCTING_CONTROLLER){
        //Ignore any calls to set whilst deleting.
        return;
    }

    if(!parent){
        qCritical() << "view_UpdateData() Cannot Update Data for NULL GraphML object.";
        return;
    }

    //Construct an Action to reverse the update
    EventAction action = getEventAction();
    action.parentID = parent->getID();
    action.Action.type = MODIFIED;
    action.Action.kind = GRAPHML_KIND::DATA;
    action.Data.keyName = keyName;

    Node* node = (Node*) parent;
    if(parent->gotData(keyName)){
        action.ID = parent->getID();
        action.Data.oldValue = parent->getDataValue(keyName);

        if(keyName == "x" || keyName == "y"){
            //Changing to -1?
            if(dataValue == -1){
                return;
            }
        }

        if(dataValue == action.Data.oldValue){
            //Don't update if we have got the same value in the model.
            //return;
        }

        parent->setDataValue(keyName, dataValue);
        
        action.Data.newValue = parent->getDataValue(keyName);
    }else{
        _attachData(parent, keyName, dataValue, true);
        return;
    }
    if(addAction){
        addActionToStack(action, addAction);
    }
}

/**
 * @brief NewController::attachData - Attaches a Data to a Entity
 * @param parent - The Entity to attach the Data to.
 * @param data - The Data to attach
 * @param addAction - Add an undo state
 */
void ModelController::attachData(Entity *parent, Data *data, bool addAction)
{
    if(parent && data){
        QString key_name = data->getKeyName();

        //Construct an Action to reverse the action
        EventAction action = getEventAction();
        action.ID = data->getID();
        action.parentID = parent->getID();
        action.Action.type = CONSTRUCTED;
        action.Action.kind = GRAPHML_KIND::DATA;
        action.Data.keyName = key_name;
        action.Data.oldValue = data->getValue();

        //Attach the Data to the parent
        parent->addData(data);

        //Set the New value of the undo action
        action.Data.newValue = parent->getDataValue(key_name);


        //Add an action to the stack.
        addActionToStack(action, addAction);
    }else{
        qCritical() << "attachData() parent or data is NULL!";
    }
}

/**
 * @brief NewController::destructData - Removes and destroys a Data attached to an Entity
 * @param parent - The Entity the Data is attached to.
 * @param keyName - The Name of the Key of the Data
 * @param addAction - Add an undo state
 */
bool ModelController::destructData(Entity *parent, QString keyName, bool addAction)
{  
    if(!parent){
        qCritical() << "destructData() parent is NULL!";
        return false;
    }

    //Get the Data from the GraphML
    Data* data = parent->getData(keyName);

    if(!data){
        qCritical() << "destructData(): " + parent->toString() + " does not contain Data for key: " + keyName;
        return false;
    }

    //Construct an Action to reverse the update
    EventAction action = getEventAction();
    action.ID = data->getID();
    action.parentID = parent->getID();
    action.Action.type = DESTRUCTED;
    action.Action.kind = GRAPHML_KIND::DATA;
    action.Data.keyName = keyName;
    action.Data.oldValue = data->getValue();

    //Remove the Data to the parent
    parent->removeData(data);
    delete data;

    //Add an action to the stack.
    addActionToStack(action, addAction);
    return true;
}


void ModelController::updateUndoRedoState()
{
    int undos = undoActionStack.size();
    int redos = redoActionStack.size();

    if(undos <= 1 || redos <= 1){
        emit undoRedoChanged();
    }
}

Node* ModelController::construct_temp_node(Node* parent_node, NODE_KIND node_kind){
    Node* node = 0;
    if(parent_node){
        //Construct node with default data
        node = entity_factory->createTempNode(node_kind);
        
        if(node){
            if(!parent_node->addChild(node)){
                //Free memory
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

Node* ModelController::construct_node(Node* parent_node, NODE_KIND node_kind){
    Node* node = 0;
    node = check_for_existing_node(parent_node, node_kind);
    if(!node){
        //Construct node with default data
        node = entity_factory->createNode(node_kind);   
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
                entity_factory->destructNode(node);
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
        auto edge = entity_factory->createEdge(source, destination, edge_kind);
        
        if(!edge){
            //Free the memory
            entity_factory->destructNode(source);
            source = 0;
        }else{
            constructNodeGUI(source);
            constructEdgeGUI(edge);
        }
    }
    return source;
}

//SIGNAL
void ModelController::constructNode(int parent_id, NODE_KIND kind, QPointF pos)
{
    //Add undo step
    triggerAction("Constructing Child Node");
    
    auto parent_node = getNode(parent_id);

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
        default:
            node = construct_child_node(parent_node, kind);
    }

    if(node){
        //Use position?
        _attachData(node, "x", pos.x());
        _attachData(node, "y", pos.y());
    }
}

void ModelController::constructWorkerProcess(int parent_id, int dst_id, QPointF pos){
    //Add undo step
    triggerAction("Constructing Worker Process");
    
    auto parent_node = getNode(parent_id);
    auto worker_node = getNode(dst_id);
    if(parent_node && worker_node && worker_node->getNodeKind() == NODE_KIND::WORKER_PROCESS){
        //clone the worker into the parent
        auto node = cloneNode(worker_node, parent_node);
        if(node){
            //Use position?
            _attachData(node, "x", pos.x());
            _attachData(node, "y", pos.y());
        }
    }
}

void ModelController::constructEdge(QList<int> src_ids, int dst_id, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    auto valid_dsts = _getConnectableNodes(getNodes(src_ids), edge_kind);
    auto dst = getNode(dst_id);
    if(dst && valid_dsts.contains(dst)){
        triggerAction("Constructing child edge");
        for(auto src_id : src_ids){
            auto src = getNode(src_id);

            auto edge = entity_factory->createEdge(src, dst, edge_kind);

            if(edge){
                constructEdgeGUI(edge);
            }else{
                break;
            }
        }
    }

    emit controller_ActionFinished();
}

void ModelController::destructEdges(QList<int> src_ids, int dst_id, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing edges");
    auto dst = getNode(dst_id);
    for(auto src_id : src_ids){
        auto src = getNode(src_id);

        auto edge = src->getEdgeTo(dst, edge_kind);
        if(edge){
            destructEdge(edge);
        }
    }
    emit controller_ActionFinished();
}

Node* ModelController::construct_setter_node(Node* parent)
{
    if(parent){
        auto node = construct_child_node(parent, NODE_KIND::SETTER);
        if(node){
            auto variable = construct_child_node(node, NODE_KIND::INPUT_PARAMETER);
            auto value = construct_child_node(node, NODE_KIND::VARIADIC_PARAMETER);

            variable->setDataValue("icon", "Variable");
            variable->setDataValue("iconPrefix", "EntityIcons");
            return node;
        }
    }
    return 0;
}


 
Node* ModelController::construct_dds_profile_node(Node* parent)
{
    if(parent){
        auto node = construct_child_node(parent, NODE_KIND::QOS_DDS_PROFILE);
        
        if(node){
            //Itterate through QOS
            /*for(auto k = NODE_KIND::QOS_DDS_POLICY_DEADLINE; k <= NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE; k++){
                auto kind = (NODE_KIND) k;
                QString kind_str = entity_factory->getNodeKindString(kind);
                constructChildNode(profile, constructDataVector(kind_str));
            }*/
            return node;
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
            condition->setDataValue("label", "Condition");
            condition->setDataValue("icon", "Condition");
            condition->setDataValue("iconPrefix", "EntityIcons");
            itteration->setDataValue("label", "Itteration");
            itteration->setDataValue("icon", "reload");
            itteration->setDataValue("iconPrefix", "Icons");

            //condition_data.append(new Data(iconKey, "Condition"));
            //condition_data.append(new Data(iconPrefixKey, "EntityIcons"));
            //itteration_data.append(new Data(iconKey, "reload"));
            //itteration_data.append(new Data(iconPrefixKey, "Icons"));
            return node;
        }
    }
    return 0;
}


void ModelController::constructConnectedNode(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, QPointF pos)
{
    QWriteLocker lock(&lock_);

    Node* parent = getNode(parentID);
    Node* dst = getNode(dstID);

    if(parent && dst){
        triggerAction("Constructed Connected Node");
        auto node = construct_connected_node(parent, nodeKind, dst, edgeKind);
        if(node){
            //Use position?
            _attachData(node, "x", pos.x());
            _attachData(node, "y", pos.y());
        }
    }
    emit controller_ActionFinished();
}





Edge* ModelController::constructEdgeWithData(EDGE_KIND edgeClass, Node *src, Node *dst, QList<Data *> data, int previousID)
{
    Edge* edge = entity_factory->createEdge(src, dst, edgeClass);
    if(edge){
        _attachData(edge, data, false);

        //If we are Undo-ing or Redo-ing and we have an ID to link it to.
        if((UNDOING || REDOING) && previousID != -1){
            link_ids(previousID, edge->getID());
        }
        constructEdgeGUI(edge);
    }
    return edge;
}

QList<Node *> ModelController::getNodes()
{
    return getNodes(node_ids_);
}

QList<Node *> ModelController::getNodes(QList<int> ids)
{
    QList<Node* > nodes;
    for(auto id: ids){
        auto node = getNode(id);
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
    updateUndoRedoState();
}

void ModelController::undo()
{
    if(canUndo()){
        emit showProgress(true, "Undoing");
        undoRedo(UNDO, true);
        emit showProgress(false);
    }
    emit controller_ActionFinished();
}

void ModelController::redo()
{
    if(canRedo()){
        emit showProgress(true, "Redoing");
        undoRedo(REDO, true);
        emit showProgress(false, "Redoing");
    }
    emit controller_ActionFinished();
}

void ModelController::openProject(QString filePath, QString xmlData)
{
    lock_.lockForWrite();
    OPENING_PROJECT = true;
    emit showProgress(true, "Opening Project");
    bool result = _newImportGraphML(xmlData, model);
    OPENING_PROJECT = false;
    lock_.unlock();

    //Update the project filePath
    setProjectPath(filePath);
    //Clear the Undo/Redo History.
    clearHistory();
    //Loading a project means we are in state with the savefile.
    setProjectDirty(false);

    //Hide the progress bar.
    emit showProgress(false);

    emit controller_ActionFinished(result, "Project couldn't be opened.");
    emit controller_OpenFinished(result);
}


/**
 * @brief NewController::copy - Attempts to copy a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void ModelController::copy(QList<int> IDs)
{

    lock_.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canCopy(selection)){
        QString value = _copy(selection);

        if(!value.isEmpty()){
            emit controller_SetClipboardBuffer(value);
            success = true;
        }
    }

    lock_.unlock();
    emit controller_ActionFinished(success, "Cannot copy selection.");
}

/**
 * @brief NewController::remove - Attempts to remove a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void ModelController::remove(QList<int> IDs)
{
    lock_.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    if(canRemove(selection)){

        triggerAction("Removing Selection");
        bool success = _remove(selection);
        emit controller_ActionFinished(success, "Cannot delete all selected entities.");
    } else {
        emit controller_ActionFinished();
    }
    lock_.unlock();
}

void ModelController::setReadOnly(QList<int> IDs, bool readOnly)
{
    lock_.lockForWrite();
    Key* readOnlyKey = entity_factory->GetKey("readOnly", QVariant::Bool);


    QList<Entity*> items;
    //Construct a list of Nodes to be snippeted
    foreach(Entity* item, getOrderedSelection(IDs)){
        if(!items.contains(item)){
            items.append(item);
            if(item->isNode()){
                Node* node = (Node*) item;
                foreach(Node* child, node->getChildren()){
                    if(!items.contains(child)){
                        items += child;
                    }
                }
                foreach(Edge* edge, node->getEdges(-1, EDGE_KIND::NONE)){
                    if(!items.contains(edge)){
                        items += edge;
                    }
                }
            }
        }
    }
    //Attach read Only Data to the top.

    bool displayWarning = true;
    //Attach read only Data.
    foreach(Entity* item, items){
        if(item->isSnippetReadOnly() || item->gotData("readOnlyDefinition")){
            if(displayWarning){
                displayWarning = false;
                //NotificationManager::manager()->displayNotification("Entity in selection is a read-only snippet. Cannot modify read-only state.", "", "", item->getID(), NS_WARNING);
            }
            continue;
        }
        Data* readOnlyData = item->getData(readOnlyKey);

        if(!readOnlyData){
            readOnlyData = new Data(readOnlyKey, readOnly);
            attachData(item, readOnlyData);
        }else{
            _setData(item, "readOnly", readOnly);
        }
    }
    lock_.unlock();
    emit controller_ActionFinished(true);
}

/**
 * @brief NewController::clear
 */
void ModelController::clear()
{
    bool success = _clear();
    emit controller_ActionProgressChanged(100);
    emit controller_ActionFinished(success);
}

/**
 * @brief NewController::replicate Essentially copies and pastes the ID's in place.
 * @param IDs - The list of entity IDs
 */
void ModelController::replicate(QList<int> IDs)
{
    lock_.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canReplicate(selection)){
        success = _replicate(selection);
    }
    lock_.unlock();
    emit controller_ActionFinished(success, "Cannot Replicate selection.");
}

/**
 * @brief NewController::cut Copies a selection of IDs and then deletes them.
 * @param selectedIDs - The list of entity IDs
 */
void ModelController::cut(QList<int> IDs)
{
    lock_.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canCut(selection)){
        QString data = _copy(selection);
        emit controller_SetClipboardBuffer(data);
        emit triggerAction("Cutting Selection");
        success = _remove(selection);
    }

    lock_.unlock();
    emit controller_ActionFinished(success, "Cannot cut selection.");
}



/**
 * @brief NewController::paste Tells the Controller to Paste
 * @param ID - The ID of the node to paste into
 * @param xmlData - The GraphML Data to paste.
 */
void ModelController::paste(QList<int> IDs, QString xmlData)
{
    lock_.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canPaste(selection)){
        success = _paste(selection.first()->getID(), xmlData);
    }
    lock_.unlock();
    emit controller_ActionFinished(success, "Cannot paste into selection");
}

/**
 * @brief NewController::_paste Pastes graphml Data into the Node specified by the ID provided.
 * @param ID - The ID of the node to paste into
 * @param xmlData - The GraphML Data to paste.
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_paste(int ID, QString xmlData, bool addAction)
{
    bool success = true;

    Node* parentNode = getNode(ID);
    if(!parentNode){
        //NotificationManager::manager()->displayNotification("No entity selected to paste into.");
        success = false;
    }else{
        if(isGraphMLValid(xmlData) && xmlData != ""){
            PASTE_USED = true;
            if(addAction){
                triggerAction("Pasting Selection.");
                emit controller_ActionProgressChanged(0, "Pasting selection");
            }

            //Paste it into the current Selected Node,
            success = _newImportGraphML(xmlData, parentNode);
            CUT_USED = false;
            PASTE_USED = false;
        }
    }
    return success;
}

/**
 * @brief NewController::_cut - Copies (to GraphML) a selection of GraphML Entities from their IDs and then deletes them.
 * @param IDs - The ID's of the entities to cut.
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_cut(QList<int> IDs, bool addAction)
{
    bool success = true;

    //If we have copied some nodes.
    if(_copy(IDs)){
        if(addAction){
            triggerAction("Cutting Selected IDs");
            emit controller_ActionProgressChanged(0, "Cutting selection");
        }
        CUT_USED = true;
        _remove(IDs, false);
    }else{
        success = false;
    }

    return success;
}

/**
 * @brief NewController::_copy - Copies (to GraphML) a selection of GraphML Entities from their IDs
 * @param IDs - The ID's of the entities to copy.
 * @param triggerAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_copy(QList<int> IDs)
{
    bool success = false;
    if(canCopy(IDs)){

        CUT_USED = false;
        //Export the GraphML for those Nodes.
        QString result = _exportGraphMLDocument(IDs, false, true);

        //Tell the view to place the resulting GraphML String into the Copy buffer.
        emit controller_SetClipboardBuffer(result);

        success = true;
    } else {
        //NotificationManager::manager()->displayNotification("Cannot copy selection.");
    }
    return success;
}

/**
 * @brief NewController::_remove - Removes the selection of GraphML Entities from their IDs
 * @param IDs - The ID's of the entities to remove.
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_remove(QList<int> IDs, bool addAction)
{

    bool allSuccess = true;

    if(IDs.length() > 0){
        controller_SetViewEnabled(false);

        if(addAction){
            triggerAction("Removing Selection");
            emit controller_ActionProgressChanged(0,"Removing selection");
        }

        int deleted=0;
        while(!IDs.isEmpty()){
            int ID = IDs.takeFirst();
            //Clear the list of related IDs.

            if(!destructEntity(ID)){
                allSuccess = false;
            }

            //Add any related ID's which need deleting to the top of the stack.
            deleted++;
            if(IDs.count() > 0){
                emit controller_ActionProgressChanged((deleted/IDs.count() * 100));
            }
        }
        emit controller_ActionProgressChanged(100);
        controller_SetViewEnabled(true);
        //success = true;
    }
    return allSuccess;
}

bool ModelController::_remove(QList<Entity *> items)
{
    bool allSuccess = true;

    int totalItems = items.size();
    while(!items.isEmpty()){
        Entity* item = items.takeFirst();

        if(!destructEntity(item)){
            allSuccess = false;
            break;
        }

        double percentageComplete = (1.0 - (items.size() / totalItems)) * 100;

        emit controller_ActionProgressChanged(percentageComplete);
    }
    return allSuccess;
}

bool ModelController::_remove(int ID, bool addAction)
{
    QList<int> IDs;
    IDs << ID;
    return _remove(IDs, addAction);
}

bool ModelController::_replicate(QList<Entity *> items)
{
    QString data = _copy(items);

    Entity* item = items.first();
    if(item->isNode()){
        Node* node = (Node*) item;
        if(node->getParentNode()){
           return _paste(node->getParentNodeID(), data);
        }
    }
    return false;
}

/**
 * @brief NewController::_replicate
 * @param IDs
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_replicate(QList<int> IDs, bool addAction)
{
    bool success = false;

    if(canCopy(IDs)){
        REPLICATE_USED = true;
        Node* node = getFirstNodeFromList(IDs);
        if(node && node->getParentNode()){
            emit controller_ActionProgressChanged(0,"Replicating selection");
            //Export the GraphML
            QString graphml = _exportGraphMLDocument(IDs, false, true);
            if(addAction){
                triggerAction("Replicating Selection");
            }
            //Import the GraphML
            if(node->getParentNode()){

                success = _paste(node->getParentNodeID(),graphml, false);
            }
            emit controller_ActionProgressChanged(100);
        }
        REPLICATE_USED = false;
    }
    return success;
}

/**
 * @brief NewController::_importProjects
 * @param xmlDataList
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_importProjects(QStringList xmlDataList, bool addAction)
{
    bool success = true;

    if(xmlDataList.length() > 0){
        controller_SetViewEnabled(false);
        if(addAction){
            triggerAction("Importing GraphML Projects.");
            emit controller_ActionProgressChanged(0, "Importing GraphML files");
        }

        foreach(QString xmlData, xmlDataList){
            //bool result = _importGraphMLXML(xmlData, model);
            bool result = _newImportGraphML(xmlData, model);
            if(!result){
                ////NotificationManager::manager()->displayNotification("Cannot import project.", "", "", -1, NS_ERROR);
                success = false;
            }
        }

        controller_SetViewEnabled(true);
    }
    return success;
}

/**
 * @brief NewController::_importSelectionSnippet Imports a Snippet into a selection.
 * @param IDs The selected ID to import into.
 * @param fileName The name of the snippet filename.
 * @param fileData The GraphML data in the snippet.
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool ModelController::_importSnippet(QList<int> IDs, QString fileName, QString fileData, bool addAction)
{
    bool success = false;
    if(canImportSnippet(IDs)){
        Node* parent = getSingleNode(IDs);
        if(parent){
            QStringList fileNameSplit = fileName.split('.');
            if(fileNameSplit.length() != 3){
                return false;
            }

            QString userFileName = fileNameSplit[0];
            QString fileParentKind = fileNameSplit[1];
            QString fileFormat = fileNameSplit[2];


            //Valide the fileParentKind
            if(fileParentKind != parent->getDataValue("kind")){
                return false;
            }

            //Validate fileFormat
            if(fileFormat != "graphml" && fileFormat != "snippet"){
                return false;
            }

            if(addAction){
                triggerAction("Importing Snippet: " + userFileName);
                emit controller_ActionProgressChanged(0,"Importing snippets");
            }
            IMPORTING_SNIPPET = true;
            success = _newImportGraphML(fileData, parent);//, false, false);
            if(!success){

            }
            IMPORTING_SNIPPET = false;
        }
    }
    return success;
}

/**
 * @brief NewController::_exportSnippet Exports a selection of GraphML entities.
 * @param IDs - The IDs of the entities to try and export.
 * @return Action successful.
 */
QString ModelController::_exportSnippet(QList<int> IDs)
{
    QString snippetData;
    if(canExportSnippet(IDs)){
        CUT_USED = false;

        QString parentNodeKind = "";

        Node* node = getFirstNodeFromList(IDs);
        if(node && node->getParentNode()){
            parentNodeKind = node->getParentNode()->getNodeKindStr();
        }

        bool readOnly = false;

        //Check if read only.
        if(parentNodeKind == "InterfaceDefinitions"){
            readOnly = askQuestion("Export as Read-Only Snippet?", "Would you like to export the current selection as a read-only snippet?");
        }


        //Construct the Keys to attach to the nodes to export.
        QList<Node*> nodeList;

        if(readOnly){
            //Get the information about this machine/time
            uint exportTimeStamp = getTimeStampEpoch();
            long long machineID = getMACAddress();

            //Construct the Keys for the data we need to attach.
            Key* readOnlyDefinitionKey = entity_factory->GetKey("readOnlyDefinition", QVariant::Bool);
            Key* readOnlyKey = entity_factory->GetKey("readOnly", QVariant::Bool);
            Key* IDKey = entity_factory->GetKey("snippetID", QVariant::Int);

            Key* timeKey = entity_factory->GetKey("snippetTime", QVariant::LongLong);
            Key* exportTimeKey = entity_factory->GetKey("exportTime", QVariant::LongLong);
            Key* macKey = entity_factory->GetKey("snippetMAC", QVariant::LongLong);


            //Construct a list of Nodes to be snippeted
            foreach(int ID, IDs){
                Node* node = getNode(ID);
                if(node && !nodeList.contains(node)){
                    nodeList += node;

                    //Add all the children (recursively) to the list of nodes to set as read-only
                    foreach(Node* child, node->getChildren()){
                        if(!nodeList.contains(child)){
                            nodeList += child;
                        }
                    }
                }
            }

            long long historicSnippetTime = exportTimeStamp;
            //Attach read only Data to all nodes in list.
            foreach(Node* node, nodeList){
                Data* readOnlyData = node->getData(readOnlyKey);
                Data* idData = node->getData(IDKey);
                Data* timeData = node->getData(timeKey);
                Data* macData = node->getData(macKey);
                Data* exportTimeData = node->getData(exportTimeKey);

                //Remove stuff.
                node->removeData(readOnlyDefinitionKey);

                //If node doesn't have snippetTime data, create and set one.
                if(!timeData){
                    timeData = new Data(timeKey, historicSnippetTime);
                    node->addData(timeData);
                }else{
                    //If we have a timestamp value which is different to the export time stamp, update.
                    if(historicSnippetTime == exportTimeStamp){
                        historicSnippetTime = timeData->getValue().toLongLong();
                    }
                }

                //If node doesn't have snippetMAC data, create and set one.
                if(!macData){
                    macData = new Data(macKey, machineID);
                    node->addData(macData);
                }

                //If node doesn't have snippetMAC data, create one.
                if(!exportTimeData){
                    exportTimeData = new Data(exportTimeKey);
                    node->addData(exportTimeData);
                }
                //Update exportTime so that the snippet loaded can be differentiated.
                exportTimeData->setValue(exportTimeStamp);

                //If node doesn't have readOnly data, create one.
                if(!readOnlyData){
                    readOnlyData = new Data(readOnlyKey);
                    node->addData(readOnlyData);
                }


                //Set Node as Read Only.
                readOnlyData->setValue(true);

                //If node doesn't have snippetID data, create and set one.
                if(!idData){
                    idData = new Data(IDKey);
                    node->addData(idData);
                    idData->setValue(node->getID());
                }
            }
        }

        //Export the GraphML for those Nodes.
        snippetData = _exportGraphMLDocument(IDs, false, false, true);

        if(readOnly){
            Key* readOnlyDefinitionKey = entity_factory->GetKey("readOnlyDefinition", QVariant::Bool);
            Key* readOnlyKey = entity_factory->GetKey("readOnly", QVariant::Bool);

            //Remove attached Data.
            foreach(Node* node, nodeList){
                //Remove the read-only data
                node->removeData(readOnlyKey);

                //Get existing readOnlyDefinition data, if not create one.
                Data* readOnlyDefData = node->getData(readOnlyDefinitionKey);
                if(!readOnlyDefData){
                    readOnlyDefData = new Data(readOnlyDefinitionKey);
                    node->addData(readOnlyDefData);
                }
                readOnlyDefData->setValue(true);
            }
        }
    }
    return snippetData;
}

long long ModelController::getMACAddress()
{
    long long returnAddress = -1;
    QString macAddress;
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces()){
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack)){
            macAddress = netInterface.hardwareAddress();
            break;
        }
    }

    if(!macAddress.isEmpty()){
        bool okay = false;
        macAddress = macAddress.replace(":", "");
        long long addr = macAddress.toLongLong(&okay, 16);
        if(okay){
            returnAddress = addr;
        }
    }
    return returnAddress;
}



QList<int> ModelController::getConnectableNodeIDs(QList<int> srcs, EDGE_KIND edgeKind)
{
    QReadLocker lock(&lock_);
    
    QList<int> dst_ids;
    
    for(auto node : _getConnectableNodes(getNodes(srcs), edgeKind)){
        dst_ids << node->getID();
    }
    return dst_ids;
}

QList<int> ModelController::getConstructableConnectableNodes(int parentID, NODE_KIND kind, EDGE_KIND edge_kind)
{
    QList<int> dst_ids;

    QWriteLocker lock(&lock_);
    auto parent_node = getNode(parentID);
    auto temp_node = construct_temp_node(parent_node, kind);
    if(temp_node){
        for(auto dst : _getConnectableNodes(QList<Node*>{temp_node}, edge_kind)){
            dst_ids << dst->getID();
        }
        //Free temp
        entity_factory->destructNode(temp_node);
    }
    return dst_ids;
}

QList<Node *> ModelController::_getConnectableNodes(QList<Node *> sourceNodes, EDGE_KIND edgeKind)
{
    QList<Node*> validNodes;

    bool tryBackwards = edgeKind == EDGE_KIND::ASSEMBLY;

    foreach(Node* src, sourceNodes){
        if(!src->requiresEdgeKind(edgeKind)){
            if(!src->acceptsEdgeKind(edgeKind) && !tryBackwards){
                return validNodes;
            }
        }
    }

    if(!sourceNodes.isEmpty()){
        //Itterate through all nodes.
        foreach(Node* dst, getNodes()){
            if(dst->acceptsEdgeKind(edgeKind)){
                //Ignore nodes which can't take this edge class.
                bool accepted = true;
                foreach(Node* src, sourceNodes){
                    if(src->canAcceptEdge(edgeKind, dst)){
                        continue;
                        //Do Nothing.
                    }else if(tryBackwards && dst->canAcceptEdge(edgeKind, src)){
                        continue;
                    }
                    accepted = false;
                    break;
                }
                if(accepted){
                    validNodes.append(dst);
                }
            }
        }
    }
    return validNodes;
}

QList<int> ModelController::getOrderedSelectionIDs(QList<int> selection)
{
    QList<int> orderedSelection;

    foreach(Entity* item, getOrderedSelection(selection)){
        if(item){
            orderedSelection.append(item->getID());
        }
    }
    return orderedSelection;
}

QList<int> ModelController::getWorkerFunctions()
{
    QReadLocker lock(&lock_);
    QList<int> ids;

    auto worker_processes = workerDefinitions->getChildrenOfKind(NODE_KIND::WORKER_DEFINITIONS);
    for(auto worker_process : worker_processes){
        ids << worker_process->getID();
    }
    return ids;
}

QList<Entity*> ModelController::getOrderedSelection(QList<int> selection)
{
    QList<Entity*> orderedSelection;

    foreach(int ID, selection){
        Entity* entity = getEntity(ID);
        if(!entity){
            continue;
        }

        Node* node = 0;
        Edge* edge = 0;
        Node* src = 0;
        Node* dst = 0;

		if(entity->isNode()){
			node = (Node*) entity;
		}else if(entity->isEdge()){
			edge = (Edge*) entity;
			src = edge->getSource();
			dst = edge->getDestination();
		}

        bool append = true;

        foreach(Entity* item, orderedSelection){
            Node* selNode = 0;
            Edge* selEdge = 0;
            if(item->isNode()){
                selNode = (Node*) item;
            }else{
                selEdge = (Edge*) item;
            }

            if(node){
                if(selNode){
                    if(selNode->isAncestorOf(node)){
                        //Can't select a child.
                        append = false;
                        break;
                    }else if(node->isAncestorOf(selNode)){
                        //Remove children
                        orderedSelection.removeAll(selNode);
                    }
                }else if(selEdge){
                    Node* sSrc = selEdge->getSource();
                    Node* sDst = selEdge->getDestination();
                    if(sSrc->isAncestorOf(node) && sDst->isAncestorOf(node)){
                        //Can't select a child
                        append = false;
                        break;
                    }else if(node->isAncestorOf(sSrc) && node->isAncestorOf(sDst)){
                        //Remove children
                        orderedSelection.removeAll(selEdge);
                    }
                }
            }else if(edge){
                if(selNode){
                    if(selNode->isAncestorOf(src) && selNode->isAncestorOf(dst)){
                        //Can't select a child
                        append = false;
                        break;
                    }
                }

            }
        }
        if(append){
            orderedSelection.append(entity);
        }
    }
    return orderedSelection;
}

QList<QVariant> ModelController::getValidKeyValues(int id, QString key_name)
{
    QReadLocker lock(&lock_);
    QList<QVariant> valid_values;
    
    Key* key = entity_factory->GetKey(key_name);
    if(key){
        auto node = getNode(id);
        if(node){
            valid_values = key->getValidValues(node->getNodeKind());
        }
    }
    return valid_values;
}

int ModelController::getSharedParent(int ID, int ID2){
    QReadLocker lock(&lock_);
    int parent_id = -1;
    auto node_1 = getNode(ID);
    auto node_2 = getNode(ID2);
    if(node_1 && node_2){
        auto parent = node_1->getCommonAncestor(node_2);
        if(parent){
            parent_id = parent->getID();
        }
    }
    return parent_id;
}

bool ModelController::isNodeAncestor(int ID, int ID2){
    QReadLocker lock(&lock_);
    bool is_ancestor = false;
    auto node_1 = getNode(ID);
    auto node_2 = getNode(ID2);
    if(node_1 && node_2){
        is_ancestor = node_1->isAncestorOf(node_2);
    }
    return is_ancestor;
}

int ModelController::getNodeParentID(int ID){
    QReadLocker lock(&lock_);
    int parent_id = -1;
    auto node = getNode(ID);
    
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
    auto node = getNode(ID);
    
    if(node){
        aspect = node->getViewAspect();
    }
    return aspect;
}

QStringList ModelController::getEntityKeys(int ID){
    QReadLocker lock(&lock_);
    QStringList keys;

    auto entity = getEntity(ID);
    if(entity){
        keys = entity->getKeyNames();
    }
    return keys;
}

QStringList ModelController::getProtectedEntityKeys(int ID){
    QReadLocker lock(&lock_);
    QStringList keys;

    auto entity = getEntity(ID);
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

    auto entity = getEntity(ID);
    if(entity){
        value = entity->getDataValue(key_name);
    }
    return value;
}


bool ModelController::isNodeOfType(int ID, NODE_TYPE type){
    QReadLocker lock(&lock_);
    bool is_type = false;
    auto node = getNode(ID);
    if(node){
        is_type = node->isNodeOfType(type);
    }
    return is_type;
}

QList<int> ModelController::getInstances(int ID)
{
    QList<int> instances;
    Node* node = getNode(ID);
    if(node){
        foreach(Node* instance, node->getInstances()){
            if(instance){
                instances << instance->getID();
            }
        }
    }
    return instances;
}

int ModelController::getAggregate(int ID)
{
    int aggrID = -1;
    Node* node = getNode(ID);
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
    Node* node = getNode(ID);
    if(node){
        for(auto edge : node->getEdges(0, EDGE_KIND::DEPLOYMENT)){
            hw_id = edge->getDestinationID();
        }
    }
    return hw_id;
}

void ModelController::setProjectSaved(QString path)
{
    setProjectDirty(false);
    if(path != ""){
        //Update the file save path.
        setProjectPath(path);
    }
}


QString ModelController::getXMLAttribute(QXmlStreamReader &xml, QString attributeID)
{
    //Get the Attributes of the current XML entity.
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attributeID)){
        return attributes.value(attributeID).toString();
    }else{
        qCritical() << "Cannot find Attribute Key: " << attributeID;
        return "";
    }
}

void ModelController::storeEntity(Entity* item)
{
    if(!item){
        return;
    }

    int ID = item->getID();

    if(!entity_hash_.contains(ID)){
        //Add it to hash.
        entity_hash_[ID] = item;

        QString kind = item->getDataValue("kind").toString();

        GRAPHML_KIND entityKind = item->getGraphMLKind();
        Node* node = 0;
        Edge* edge = 0;
        if(entityKind == GRAPHML_KIND::NODE){
            node = (Node*)item;
            node_ids_.append(ID);
        }else if(entityKind == GRAPHML_KIND::EDGE){
            edge = (Edge*)item;
            edge_ids_.append(ID);
        }

        //Connect things!
        connect(item, &Entity::dataChanged, this, &ModelController::dataChanged);
        connect(item, &Entity::dataRemoved, this, &ModelController::dataRemoved);

    
        if(entityKind == GRAPHML_KIND::NODE){
            //Construct Node
            emit NodeConstructed(node->getParentNodeID(), ID, node->getNodeKind());
        }else if(entityKind == GRAPHML_KIND::EDGE){
            //Construct Node
            emit EdgeConstructed(ID, edge->getEdgeKind(), edge->getSourceID(), edge->getDestinationID());
        }
    }
}

Entity *ModelController::getEntity(int ID)
{
    if(entity_hash_.contains(ID)){
        return entity_hash_[ID];
    }else{
        auto linked_id = get_linked_ids(ID);
        if(linked_id != ID){
            return getEntity(linked_id);
        }else{
            return 0;
        }
    }
}

Node* ModelController::getNode(int ID){
    auto entity = getEntity(ID);
    if(entity && entity->isNode()){
        return (Node*)entity;
    }else{
        return 0;
    }
}

Edge* ModelController::getEdge(int ID){
    auto entity = getEntity(ID);
    if(entity && entity->isEdge()){
        return (Edge*)entity;
    }else{
        return 0;
    }
}



void ModelController::removeEntity(int ID)
{
    
    if(entity_hash_.contains(ID)){
        Entity* item = entity_hash_[ID];
        if(item){

            GRAPHML_KIND ek = item->getGraphMLKind();
            emit entityDestructed(ID, ek, "");
        }
    }

    if(readOnlyHash.containsValue(ID)){
        readOnlyHash.removeValue(ID);
    }

    if(reverseReadOnlyLookup.contains(ID)){
        int originalID = reverseReadOnlyLookup[ID];
        reverseReadOnlyLookup.remove(ID);
        readOnlyLookup.remove(originalID);
    }

    entity_hash_.remove(ID);

    node_ids_.removeOne(ID);
    edge_ids_.removeOne(ID);

     


    if(entity_hash_.size() != (node_ids_.size() + edge_ids_.size())){
        qCritical() << "Hash Map Inconsistency detected!";
    }
}


bool ModelController::attachChildNode(Node *parentNode, Node *node, bool notify_view)
{
    if(parentNode->addChild(node)){
        if(notify_view){
            constructNodeGUI(node);
        }

         if(isUserAction()){
            ReadOnlyState nodeState = getReadOnlyState(node);

            //If the node is a definition, construct an instance/Implementation in each Instance/Implementation of the parentNode.
            foreach(Node* dependant, parentNode->getDependants()){
                ReadOnlyState dependantState = getReadOnlyState(dependant);
                if(nodeState.isValid() && dependantState.isValid()){
                    if(nodeState.snippetMAC == dependantState.snippetMAC && nodeState.snippetTime == dependantState.snippetTime){
                        //If we have to construct into a read-only node which shares the same MAC and Time, assume the read only snippet contains an item in document, so don't autoconstruct.
                        continue;
                    }
                }
                //Setup dependant
                constructDependantRelative(dependant, node);
            }
         }
         return true;
    }
    return false;
}


bool ModelController::updateProgressNotification()
{
    if(OPENING_PROJECT || IMPORTING_PROJECT){
           return true;
   }
    return false;
}

QList<int> ModelController::getIDs(QList<Entity *> items)
{
    QList<int> IDs;
    foreach(Entity* item, items){
        IDs.append(item->getID());
    }
    return IDs;
}

QList<Entity *> ModelController::getEntities(QList<int> IDs)
{
    return getOrderedSelection(IDs);
}

Node *ModelController::cloneNode(Node *original, Node *parent)
{
    if(original && parent){
        auto node = construct_child_node(parent, original->getNodeKind());
        
        if(node){
            //Get the data
            for(auto data : original->getData()){
                _attachData(node, data->getKeyName(), data->getValue());
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
    bool isInstance = parent->isInstance();
    int nodesMatched = 0;

    NODE_KIND dependant_kind = NODE_KIND::NONE;

    if(isInstance){
        dependant_kind = definition->getInstanceKind();
    }else{
        dependant_kind = definition->getImplKind();
    }

    //For each child in parent, check to see if any Nodes match Label/Type
    foreach(Node* child, parent->getChildrenOfKind(dependant_kind, 0)){        
        Node* childDef = child->getDefinition();

        if(childDef){
            if(childDef == definition){
                nodesMatched ++;
            }
            //Move onto non-definition'd children.
            continue;
        }

        bool labelMatched = child->compareData(definition, "label");
        bool typeMatched = child->compareData(definition, "type");

        if(!typeMatched){
            if(child->getDataValue("type") == definition->getDataValue("label")){
                typeMatched = true;
            }
        }

        if(typeMatched && labelMatched){
            Edge* edge = entity_factory->createEdge(child, definition, EDGE_KIND::DEFINITION);

            if(!edge){
                qCritical() << "constructDefinitionRelative(): Couldn't construct Edge between Relative Node and Definition Node.";
                continue;
            }
            nodesMatched++;
        }
    }

    //If we didn't find a match, we must create an Instance.
    if(nodesMatched == 0){
        Node* instance_node = construct_connected_node(parent, dependant_kind, definition, EDGE_KIND::DEFINITION);
        if(!instance_node){
            return 0;
        }
        nodesMatched ++;
    }

    return nodesMatched;
}




/**
 * @brief NewController::destructNode Fully deletes a Node; Including all of it's dependants and Edges etc.
 * @param node The Node to Delete.
 * @return Success
 */
bool ModelController::destructNode(Node *node)
{
    if(!node){
        return true;
    }

    bool addAction = true;


    int ID = node->getID();
    Node* parentNode = node->getParentNode();
    int parentID = -1;
    if(parentNode){
        parentID = parentNode->getID();

    }


    if(DESTRUCTING_CONTROLLER){
        //If we are destructing the controller; Don't add an undo state.
        addAction = false;
    }else if(!node->isInModel()){
        //If the item isn't in the Model; Don't add an undo state.
        addAction = false;
    }

    if(addAction && !canDeleteNode(node)){
        //If the item is reverseable action, check if we can actually remove the node.
        return false;
    }




    //Get Dependants first.
    QList<Node*> dependants = node->getNestedDependants();

    QList<Edge*> edges = node->getAllEdges();
    //Remove all Edges.
    while(!edges.isEmpty()){
        //Go backwards to ensure that we can reconstruct the edges.
        Edge* edge = edges.takeLast();
        //qCritical() << "Destructing Edge: " << edge->toString();
        destructEdge(edge);
    }


    //Remove all nodes which depend on this.
    while(!dependants.isEmpty()){
        Node* dependant = dependants.takeFirst();
        //qCritical(    ) << "Tearing down Dependants: " << dependant;
        destructNode(dependant);
    }


    //Remove all Children.
    while(node->hasChildren()){
        Node* child = node->getFirstChild();
        destructNode(child);
    }

    if(addAction){
        //Add an action to reverse this action.
        EventAction action = getEventAction();
        action.ID = ID;
        action.parentID = parentID;
        action.Action.type = DESTRUCTED;
        action.Action.kind = node->getGraphMLKind();
        action.Entity.kind = node->getGraphMLKind();
        action.Entity.nodeKind = node->getNodeKindStr();
        action.Entity.XML = _exportGraphMLDocument(node);
        addActionToStack(action);
    }


    if(parentNode){
        //Put this item last to fix the sort order of it's siblings.
        node->setDataValue("index", parentNode->childrenCount());
    }

    removeEntity(ID);

    if(model == node){
        model = 0;
    }else if(workerDefinitions == node){
        workerDefinitions = 0;
    }

    entity_factory->destructNode(node);
    //TODO DELETE FREE
    //delete node;
    return true;
}



bool ModelController::destructEdge(Edge *edge)
{
    if(!edge){
        qCritical() << "destructEdge(): Edge is NULL";
        return true;
    }


    bool addAction = true;

    if(DESTRUCTING_CONTROLLER){
        //If we are destructing the controller; Don't add an undo state.
        addAction = false;
    }

    int ID = edge->getID();
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    if(!(src && dst)){
        qCritical() << "destructEdge(): Source and/or Destination are NULL.";
        return false;
    }

    if(addAction){
        //Add an action to reverse this action.
        EventAction action = getEventAction();
        action.ID = ID;
        action.parentID = model->getID();
        action.Action.type = DESTRUCTED;
        action.Action.kind = edge->getGraphMLKind();
        action.Entity.kind = edge->getGraphMLKind();
        QList<int> IDs;
        IDs << ID;
        action.Entity.XML = _exportGraphMLDocument(IDs, true);
        action.Entity.edgeClass = edge->getEdgeKind();
        addActionToStack(action);
    }

    //Teardown specific edge classes.
    EDGE_KIND edgeClass = edge->getEdgeKind();

    switch(edgeClass){
    case EDGE_KIND::DEFINITION:{
        teardownDependantRelationship(dst, src);
        break;
    }
    case EDGE_KIND::AGGREGATE:{
        Aggregate* aggregate = dynamic_cast<Aggregate*>(dst);
        if(aggregate){
            EventPort* eventPort = dynamic_cast<EventPort*>(src);
            if(eventPort){
                teardownEventPortAggregateRelationship(eventPort, aggregate);
            }else{
                teardownAggregateRelationship(src, aggregate);
            }
        }
        break;
    }
    case EDGE_KIND::ASSEMBLY:{
        // UnBind Topics Together.
        Data* sourceTopicName = src->getData("topicName");
        Data* destinationTopicName = dst->getData("topicName");
        if(destinationTopicName && sourceTopicName ){
            destinationTopicName->unsetParentData();
        }
        break;
    }
    case EDGE_KIND::DATA:{
        if(dst->isNodeOfType(NODE_TYPE::DATA) && src->isNodeOfType(NODE_TYPE::DATA)){
            setupDataEdgeRelationship((DataNode*)src, (DataNode*)dst, false);
        }
        break;
    }
    case EDGE_KIND::DEPLOYMENT:{
        if(isUserAction()){
            QString message = "Disconnected '" % src->getDataValue("label").toString() % "' from '" % dst->getDataValue("label").toString() % "'";
            //NotificationManager::manager()->displayNotification(message, "Icons", "cross", src->getID());
        }
    }
    default:
        break;
    }

    //Remove it from the hash of GraphML
    removeEntity(ID);

    entity_factory->destructEdge(edge);
    return true;
}

bool ModelController::destructEntity(int ID)
{
    Entity* entity = getEntity(ID);
    return destructEntity(entity);
}

bool ModelController::destructEntity(Entity *item)
{
    if(item){
        if(item->isNode()){
            return destructNode((Node*)item);
        }else if(item->isEdge()){
            return destructEdge((Edge*)item);
        }
    }
    return false;
}



bool ModelController::reverseAction(EventAction action)
{
    bool success = false;
    if(action.Action.kind == GRAPHML_KIND::NODE || action.Action.kind == GRAPHML_KIND::EDGE){

        if(action.Action.type == CONSTRUCTED){
            success = destructEntity(action.ID);
        }else if(action.Action.type == DESTRUCTED){
            Node* parentNode = getNode(action.parentID);
            if(parentNode){
                success = _newImportGraphML(action.Entity.XML, parentNode);
            }
        }
    }else if(action.Action.kind == GRAPHML_KIND::DATA){
        if(action.Action.type == CONSTRUCTED){
            Entity* entity = getEntity(action.parentID);
            if(entity){
                success = destructData(entity, action.Data.keyName);
            }
        }else if(action.Action.type == MODIFIED){
            Entity* entity = getEntity(action.parentID);

            if(entity){
                _setData(entity, action.Data.keyName, action.Data.oldValue);
                success = true;
            }
        }else if(action.Action.type == DESTRUCTED){
            Entity* entity = getEntity(action.parentID);
            if(entity){
                success = _attachData(entity, action.Data.keyName, action.Data.oldValue);
            }
        }
    }else if(action.Action.kind == GRAPHML_KIND::KEY){
    }
    return success;
}

//Attachs a list of data
bool ModelController::_attachData(Entity *item, QList<Data *> dataList, bool addAction)
{
    if(item){
        foreach(Data* new_data, dataList){
            Key* key = new_data->getKey();
            QString key_name = key->getName();

            Data* current_data = item->getData(key);

            if(current_data){
                //Set the new_data value onto the old value
                _setData(item, key->getName(), new_data->getValue(), addAction);
            }else{
                //Atach our new data
                attachData(item, new_data, addAction);
            }
        }
        return true;
    }else{
        return false;
    }
}

bool ModelController::_attachData(Entity *item, QString keyName, QVariant value, bool addAction)
{
    //Try get the key
    Key* key = entity_factory->GetKey(keyName, value.type());
    if(key){
        Data* data = new Data(key, value);
        if(data){
            return _attachData(item, data, addAction);
        }
    }
    return false;
}


bool ModelController::_attachData(Entity *item, Data *data, bool addAction)
{
    QList<Data *> dataList;
    dataList.append(data);
    return _attachData(item, dataList, addAction);
}

void ModelController::addActionToStack(EventAction action, bool useAction)
{
    //Change made, make dirty the project
    setProjectDirty(true);

    if(useAction){
        if(UNDOING){
            redoActionStack.push(action);
        }else{
            undoActionStack.push(action);
        }

        updateUndoRedoState();
    }
}

void ModelController::undoRedo(bool undo, bool updateProgess)
{
    UNDOING = undo;
    REDOING = !undo;

    //Used to store the stack of actions we are to use.
    QStack<EventAction> actionStack = redoActionStack;

    if(UNDOING){
        //Set to the use the stack.
        actionStack = undoActionStack;
    }

    //Get the total number of actions in the history stack.
    float actionCount = actionStack.size();

    if(actionCount == 0){
        //qCritical () << "No Actions in Undo/Redo Buffer.";
        return;
    }



    //Get the ID and Name of the top-most action.
    int topActionID = actionStack.top().Action.ID;
    QString topActionName = actionStack.top().Action.name;
    //Emit a new action so this Undo/Redo operation can be reverted.
    triggerAction(topActionName);

    //This vector will store all ActionItems which match topActionID
    QList<EventAction> toReverse;
    while(!actionStack.isEmpty()){
        //Get the top-most action.
        EventAction action = actionStack.top();


        //If this action has the same ID, we should undo it.
        if(action.Action.ID == topActionID){
            toReverse.append(action);
            //Remove if from the action stack.
            actionStack.pop();
        }else{
            //If we don't match, it must be a new actionID, so we are done.
            break;
        }
    }


    actionCount = toReverse.size();


    int maxRetry = 3;
    QHash<int, int> retryCount;



    previousUndos = actionCount;
    int actionsReversed = 0;
    while(!toReverse.isEmpty()){
        EventAction reverseState = toReverse.takeFirst();
        bool success = reverseAction(reverseState);
        if(!success){
            retryCount[reverseState.Action.actionID] +=1;
            if(retryCount[reverseState.Action.actionID] <= maxRetry){
                toReverse.append(reverseState);
            }

        }else{
            actionsReversed ++;

            if(updateProgess){
                emit progressChanged((actionsReversed * 100) / actionCount);
            }
        }
    }
    retryCount.clear();


    if(UNDOING){
        undoActionStack = actionStack;
    }else{
        redoActionStack = actionStack;
    }

    UNDOING = false;
    REDOING = false;
    updateUndoRedoState();
}

bool ModelController::canDeleteNode(Node *node)
{
    if(!node){
        return false;
    }
    // Added this here to stop the user from being able to cut or delete the model
    if(model == node){
        return false;
    }

    if(protectedNodes.contains(node)){
        return false;
    }


    return true;
}

/**
 * @brief NewController::readFile Reads a file and returns the contents.
 * @param filePath The path to the file to read.
 * @return first = result, second = file contents.
 */
QPair<bool, QString> ModelController::readFile(QString filePath)
{
    QPair<bool, QString> result;

    QFile file(filePath);

    result.first = file.open(QFile::ReadOnly | QFile::Text);
    if (result.first) {
        QTextStream fileStream(&file);
        result.second = fileStream.readAll();
        file.close();
    }

    return result;
}


void ModelController::clearHistory()
{
    currentActionID = 0;
    actionCount = 0;
    currentAction = "";
    undoActionStack.clear();
    redoActionStack.clear();
    updateUndoRedoState();
}

void ModelController::_projectNameChanged()
{
    if(model){
        emit controller_ProjectNameChanged(model->getDataValue("label").toString());
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

    foreach(EDGE_KIND edgeClass, entity_factory->getEdgeKinds()){
        if(src->acceptsEdgeKind(edgeClass) && dst->acceptsEdgeKind(edgeClass) && src->requiresEdgeKind(edgeClass)){
            edgeKinds << edgeClass;
        }
    }
    return edgeKinds;
}

QString ModelController::_copy(QList<Entity *> selection)
{
    QList<int> IDs = getIDs(selection);

    //Export the GraphML for those Nodes.
    QString result = _exportGraphMLDocument(IDs, false, true);

    return result;
}


void ModelController::constructNodeGUI(Node *node)
{
    if(!node){
        return;
    }

    //Construct an ActionItem to reverse Node Construction.
    EventAction action = getEventAction();
    action.Action.type = CONSTRUCTED;
    action.Action.kind = node->getGraphMLKind();
    action.ID = node->getID();
    action.parentID = node->getParentNodeID();
    action.Entity.kind = node->getGraphMLKind();
    action.Entity.nodeKind = node->getNodeKindStr();

    if(node->getParentNode()){
        //Variable.
        Node* parentNode = node->getParentNode();
        if(parentNode && parentNode->getNodeKindStr() == "Variable"){
            Data* typeData = parentNode->getData("type");
            Data* childType = node->getData("type");
            typeData->setParentData(childType);
        }
    }

    //Add the node to the list of nodes constructed.
    storeEntity(node);

    //Add Action to the Undo/Redo Stack.
    addActionToStack(action);
}

void ModelController::setupModel()
{
    //Construct the top level parents.
    model = construct_node(0, NODE_KIND::MODEL);
    workerDefinitions = construct_node(0, NODE_KIND::WORKER_DEFINITIONS);

    Data* labelData = model->getData("label");
    if(labelData){
        connect(labelData, &Data::dataChanged, this, &ModelController::_projectNameChanged);

         //Update the view with the correct Model Label.
        _projectNameChanged();
    }

    //Construct the aspects
    interfaceDefinitions = construct_child_node(model, NODE_KIND::INTERFACE_DEFINITIONS);
    behaviourDefinitions = construct_child_node(model, NODE_KIND::BEHAVIOUR_DEFINITIONS);
    deploymentDefinitions = construct_child_node(model, NODE_KIND::DEPLOYMENT_DEFINITIONS);
    assemblyDefinitions = construct_child_node(deploymentDefinitions, NODE_KIND::ASSEMBLY_DEFINITIONS);
    hardwareDefinitions = construct_child_node(deploymentDefinitions, NODE_KIND::HARDWARE_DEFINITIONS);

    //Construct the localhost node
    localhostNode = construct_child_node(hardwareDefinitions, NODE_KIND::HARDWARE_NODE);
    localhostNode->setDataValue("label", "localhost");
    localhostNode->setDataValue("ip_address", "127.0.0.1");
    localhostNode->setDataValue("os", QSysInfo::productType());
    localhostNode->setDataValue("os_version", QSysInfo::productVersion());
    localhostNode->setDataValue("arch", QSysInfo::currentCpuArchitecture());

    //Protect the nodes
    protectedNodes << model;
    protectedNodes << workerDefinitions;
    protectedNodes << interfaceDefinitions;
    protectedNodes << behaviourDefinitions;
    protectedNodes << deploymentDefinitions;
    protectedNodes << assemblyDefinitions;
    protectedNodes << hardwareDefinitions;
    protectedNodes << localhostNode;

    //construct the GUI
    for(auto node : protectedNodes){
        constructNodeGUI(node);
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
    }

    //Bind Type to either Type or Label
    
    if(bind_types){
        if(defn_type){
            child_type->setParentData(defn_type);
        }else if(defn_label){
            child_type->setParentData(defn_label);
        }
    }

    //Bind label
    if(bind_labels){
        child_label->setParentData(defn_label);
    }

    //Bind Keys
    if(bind_keys){
        child_key->setParentData(defn_key);
    }

    //Bind Index
    if(bind_index){
        child_index->setParentData(defn_index);
    }
}

void ModelController::unbindData(Node *definition, Node *instance)
{
    foreach(Data* attachedData, definition->getData()){
        Data* newData = 0;
        newData = instance->getData(attachedData->getKey());
        if(newData){
            if(newData->getParentData() == attachedData){
                newData->unsetParentData();
            }
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
bool ModelController::setupDependantRelationship(Node *definition, Node *node)
{
    //Got Aggregate Edge.
    if(!definition || !node){
        return false;
    }

    auto node_kind = node->getNodeKind();
    if(isUserAction()){
        //For each child contained in the Definition, which itself is a definition, construct an Instance/Impl inside the Parent Instance/Impl.
        foreach(Node* child, definition->getChildren(0)){
            if(child && child->isNodeOfType(NODE_TYPE::DEFINITION)){
                //Construct relationships between the children which matched the definitionChild.
                int instancesConnected = constructDependantRelative(node, child);
                
                //Couldn't establish links
                if(instancesConnected == 0){
                    //To reduce links in Assembly, we don't care if they don't have AggregateInstances
                    if(node_kind != NODE_KIND::INEVENTPORT_INSTANCE && node_kind != NODE_KIND::OUTEVENTPORT_INSTANCE){
                        qCritical() << "setupDefinitionRelationship(): Couldn't create a Definition Relative for: " << child->toString() << " In: " << node->toString();
                        return false;
                    }
                }
            }
        }
    }

    //Bind the un-protected Data attached to the Definition to the Instance.
    bindData(definition, node);

    if(!node->gotEdgeTo(definition, EDGE_KIND::DEFINITION)){
        qCritical() << "setupDefinitionRelationship(): Cannot find connecting Edge.";
        return false;
    }

    if(node->isInstance()){
        definition->addInstance(node);
    }else{
        definition->addImplementation(node);
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
bool ModelController::setupEventPortAggregateRelationship(EventPort *eventPort, Aggregate *aggregate)
{
    //Got Aggregate Edge.
    if(!(eventPort && aggregate)){
        qCritical() << "setupAggregateRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    Node* aggregateInstance = 0;


    //Only auto construct if we are processing a user action.
    if(isUserAction()){
        if(eventPort->getNodeKind() == NODE_KIND::INEVENTPORT || eventPort->getNodeKind() == NODE_KIND::OUTEVENTPORT){
            //Check for an Existing AggregateInstance in the EventPort.
            foreach(Node* child, eventPort->getChildrenOfKind(NODE_KIND::AGGREGATE_INSTANCE, 0)){
                aggregateInstance = child;
            }

            //If we couldn't find an AggregateInstance in the EventPort, construct one.
            if(!aggregateInstance){
                aggregateInstance = construct_connected_node(eventPort, NODE_KIND::AGGREGATE_INSTANCE, aggregate, EDGE_KIND::DEFINITION);
            }else{
                auto agg_defn = aggregateInstance->getDefinition();

                if(agg_defn && agg_defn != aggregate){
                    qCritical() << "setupAggregateRelationship(): EventPort already contains a defined AggregateInstance!";
                    return false;
                }

                if(!aggregateInstance->gotEdgeTo(aggregate, EDGE_KIND::DEFINITION)){
                    //Try connect
                    auto edge = entity_factory->createEdge(aggregateInstance, aggregate, EDGE_KIND::DEFINITION);
                    if(!edge){
                        qCritical() << "setupAggregateRelationship(): Edge between AggregateInstance and Aggregate wasn't constructed!";
                        return false;
                    }
                }
            }
        }
    }

    if(eventPort->gotEdgeTo(aggregate, EDGE_KIND::AGGREGATE)){
        //Edge Created Set Aggregate relation.
        eventPort->setAggregate(aggregate);

        //Set Type
        Data* eventPortType = eventPort->getData("type");
        Data* aggregateType = aggregate->getData("type");

        if(eventPortType && aggregateType){
            eventPortType->setParentData(aggregateType);
            eventPortType->setValue(aggregateType->getValue());
        }
        return true;
    }else{
        return false;
    }
}

bool ModelController::teardownEventPortAggregateRelationship(EventPort *eventPort, Aggregate *aggregate)
{
    if(!(eventPort && aggregate)){
        qCritical() << "teardownAggregateRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Unset the Aggregate
    eventPort->unsetAggregate();

    //Unset Type information;
    Data* eventPortType = eventPort->getData("type");
    if(eventPortType){
        eventPortType->unsetParentData();
        eventPortType->clearValue();
    }
    return true;
}

bool ModelController::setupAggregateRelationship(Node *node, Aggregate *aggregate)
{
    if(!(node && aggregate)){
        qCritical() << "setupVectorRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Check for a connecting Edge between the eventPort and aggregate.
    Edge* edge = node->getEdgeTo(aggregate, EDGE_KIND::AGGREGATE);


    Key* labelKey = entity_factory->GetKey("label", QVariant::String);

    //Check for the existance of the Edge constructed.
    if(!edge){
        qCritical() << "setupVectorRelationship(): Edge between Vector and Aggregate doesn't exist!";
        return false;
    }

    //Set Label of Edge.
    if(!edge->getData(labelKey)){
        Data* label = new Data(labelKey, "Uses Aggregate");
        attachData(edge, label, false);
    }
    //Set AutoGenerated.



    //Set Type
    Data* nodeType = node->getData("type");
    Data* aggregateLabel = aggregate->getData("label");

    if(nodeType && aggregateLabel){
        nodeType->setParentData(aggregateLabel);
        nodeType->setValue(aggregateLabel->getValue());
    }else{
        return false;
    }


    EventPortAssembly* eventPortDelegate = dynamic_cast<EventPortAssembly*>(node);
    if(eventPortDelegate){
        eventPortDelegate->setAggregate(aggregate);
    }

    return true;

}

bool ModelController::teardownAggregateRelationship(Node *node, Aggregate *aggregate)
{
    if(!(node && aggregate)){
        qCritical() << "teardownVectorRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Unset Type information;
    Data* nodeType = node->getData("type");
    if(nodeType){
        nodeType->unsetParentData();
        nodeType->clearValue();
    }

    if(node->isNodeOfType(NODE_TYPE::EVENTPORT_ASSEMBLY)){
        EventPortAssembly* ep = (EventPortAssembly*) node;
        ep->unsetAggregate();
    }
    return true;
}

bool ModelController::setupDataEdgeRelationship(DataNode *output, DataNode *input, bool setup)
{
    Node* inputTopParent = input->getParentNode(input->getDepthFromAspect() - 2);
    Node* outputTopParent = output->getParentNode(output->getDepthFromAspect() - 2);

    if(inputTopParent){
        //If we are connecting to an Variable, we don't want to bind.
        if(inputTopParent->getNodeKind() == NODE_KIND::VARIABLE){
            return true;
        }
    }


    Data* definitionData = output->getData("type");
    Data* valueData = input->getData("value");

    if(outputTopParent){
        //Bind Parent Label if we are a variable.
        if(outputTopParent->getNodeKind() == NODE_KIND::VARIABLE || outputTopParent->getNodeKind() == NODE_KIND::ATTRIBUTE_IMPL){
            definitionData = outputTopParent->getData("label");
        }
    }

    if(definitionData && valueData){
        if(setup){
            valueData->setParentData(definitionData);
        }else{
            valueData->unsetParentData();
        }
    }

    //Bind special stuffs.
    Node* inputParent = input->getParentNode();
    if(inputParent){
        if(inputParent->getNodeKind() == NODE_KIND::WORKER_PROCESS){
            QString workerName = inputParent->getDataValue("worker").toString();
            QString parameterLabel = input->getDataValue("label").toString();

            if(workerName == "Vector_Operations"){
                if(parameterLabel == "Vector" || parameterLabel == "VectorA"){
                    Node* vector = output;
                    Node* vector_child = 0;

                    //If Vector has children
                    if(vector->childrenCount() == 1){
                        vector_child = vector->getChildren(0)[0];
                    }

                    //Check the siblings of the input
                    foreach(Node* child, input->getSiblings()){

                        if(child && child->isNodeOfType(NODE_TYPE::PARAMETER)){
                            QString parameter_label = child->getDataValue("label").toString();

                            Data* parameter_type = child->getData("type");
                            Data* bind_type = 0;

                            if(parameter_label == "Value" || parameter_label == "DefaultValue"){
                                //Bind the Type of the child in the Vector
                                bind_type = vector_child->getData("type");
                            }else if(parameter_label == "VectorB"){
                                //Bind the Type of the actual vector.
                                bind_type = vector->getData("type");
                            }

                            if(parameter_type){
                                if(setup){
                                    if(bind_type){
                                        parameter_type->setParentData(bind_type);
                                    }
                                }else{
                                    parameter_type->unsetParentData();
                                    parameter_type->clearValue();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}


/**
 * @brief NewController::teardownDefinitionRelationship
 * Attempts to destruct the relationship between the Instance and definition provided.
 * Will remove *ALL* Instances of all Definitions contained by Definition provided. Unbinds relevant Data and will add Node to be removed.
 * @param definition - The Node which is the Definition of the relationship.
 * @param aggregate - The Node which is the Instance.
 * @param instance - Is this an Instance or Implementation Relationship.
 * @return true if Definition Relation was removed correctly.
 */
bool ModelController:: teardownDependantRelationship(Node *definition, Node *node)
{
    //Got Aggregate Edge.
    if(!(definition && node)){
        qCritical() << "teardownDefinitionRelationship(): Definition or Node is NULL.";
        return false;
    }

    //Unbind data.
    unbindData(definition, node);

    if(!definition->gotEdgeTo(node, EDGE_KIND::DEFINITION)){
        qCritical() << "teardownDefinitionRelationship(): No Edge between Definition and Node.";
        return false;
    }

    //Unset the Relationship between Definition and Instance/Impl
    if(node->isInstance()){
        definition->removeInstance(node);
    }else{
        definition->removeImplementation(node);
    }

    return true;
}

bool ModelController::isGraphMLValid(QString inputGraphML)
{
    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xmlErrorChecking(inputGraphML);

    //Check for Errors
    while(!xmlErrorChecking.atEnd()){
        xmlErrorChecking.readNext();
        //float lineNumber = xmlErrorChecking.lineNumber();
        if (xmlErrorChecking.hasError()){
            qCritical() << "isGraphMLValid(): Parsing Error! Line #" << xmlErrorChecking.lineNumber();
            qCritical() << "\t" << xmlErrorChecking.errorString();
            return false;
        }
    }
    return true;
}

void ModelController::constructEdgeGUI(Edge *edge)
{
    //Construct an ActionItem to reverse an Edge Construction.
    EventAction action = getEventAction();

    action.Action.type = CONSTRUCTED;
    action.Action.kind = edge->getGraphMLKind();
    action.ID = edge->getID();
    action.Entity.kind = edge->getGraphMLKind();

    Key* descriptionKey = entity_factory->GetKey("description", QVariant::String);


    //Get Source and Destination of the Edge.
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    //Add Action to the Undo/Redo Stack
    addActionToStack(action);

    if(!src || !dst){
        qCritical() << "Source and Desitnation null";
    }
    EDGE_KIND edgeClass = edge->getEdgeKind();


    switch(edgeClass){
    case EDGE_KIND::DEFINITION:{
        setupDependantRelationship(dst, src);
        //DefinitionEdge is either an Instance or an Impl
        break;
    }
    case EDGE_KIND::AGGREGATE:{
        if(dst->getNodeKind() == NODE_KIND::AGGREGATE){
            Aggregate* aggregate = (Aggregate*) dst;

            if(src->isNodeOfType(NODE_TYPE::EVENTPORT)){
                EventPort* eventPort = (EventPort*) src;
                setupEventPortAggregateRelationship(eventPort, aggregate);
            }else{
                setupAggregateRelationship(src, aggregate);
            }
        }
        break;
    }
    case EDGE_KIND::ASSEMBLY:{
        //Bind Topics Together, if they contain.
        Data* srcTopicName = src->getData("topicName");
        Data* dstTopicName = dst->getData("topicName");

        if(srcTopicName && dstTopicName){
            dstTopicName->setParentData(srcTopicName);
        }

        if(!edge->getData(descriptionKey)){
            Data* label = new Data(descriptionKey, "Connected To");
            attachData(edge, label, false);
        }
        break;
    }
    case EDGE_KIND::DATA:{
        if(dst->isNodeOfType(NODE_TYPE::DATA) && src->isNodeOfType(NODE_TYPE::DATA)){
            setupDataEdgeRelationship((DataNode*)src, (DataNode*)dst, true);
        }
        break;
    }
    case EDGE_KIND::DEPLOYMENT:{
        if(isUserAction()){
            QString message = "Deployed '" % src->getDataValue("label").toString() % "' from '" % dst->getDataValue("label").toString() % "'";
            //NotificationManager::manager()->displayNotification(message, "Icons", "connect", src->getID());
        }
        break;
    }
    case EDGE_KIND::QOS:{
        if(!edge->getData(descriptionKey)){
            Data* label = new Data(descriptionKey, "Using QOS");
            attachData(edge, label, false);
        }
        break;
    }
    default:
        break;
    }
    

    storeEntity(edge);
}


Node *ModelController::getFirstNodeFromList(QList<int> IDs)
{
    Node* node = 0;
    foreach(int ID, IDs){
        Entity* graphML = getEntity(ID);
        if(graphML && graphML->isNode()){
            node = (Node*)graphML;
        }
    }
    return node;
}

int ModelController::get_linked_ids(int id)
{
    auto original_id = id;
    auto new_id = id;
    while(true){
        if(id_hash_.contains(id)){
            auto temp_id = id_hash_[id];
            if(temp_id == -1){
                return id;
            }else{
                id = temp_id;
            }
        }else{
            return original_id;
        }
    }
}




void ModelController::link_ids(int old_id, int new_id)
{
    //Point the ID Hash for the oldID to the newID
    id_hash_[old_id] = new_id;
    if(!id_hash_.contains(new_id)){
        //Set the ID Hash for the newID to -1
        id_hash_[new_id] = -1;
    }
}


QString ModelController::getTimeStamp()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyyy-MM-dd hh:mm:ss");
}

uint ModelController::getTimeStampEpoch()
{
    return QDateTime::currentDateTime().toTime_t();
}


QString ModelController::getProjectAsGraphML()
{
    lock_.lockForRead();
    QString data = _exportGraphMLDocument(model);
    lock_.unlock();
    return data;
}

QString ModelController::getSelectionAsGraphMLSnippet(QList<int> IDs)
{
    lock_.lockForRead();
    QList<int> orderedIDs = getOrderedSelectionIDs(IDs);
    QString data = _exportSnippet(orderedIDs);
    lock_.unlock();
    return data;
}

QList<EDGE_KIND> ModelController::getValidEdgeKindsForSelection(QList<int> IDs)
{
    lock_.lockForRead();

    QList<Entity*> entities = getOrderedSelection(IDs);
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

    QList<Entity*> entities = getOrderedSelection(IDs);
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

QList<NODE_KIND> ModelController::getAdoptableNodeKinds2(int ID)
{
    QReadLocker lock(&lock_);
    QList<NODE_KIND> kinds;

    Node* parent = getNode(ID);

    //Ignore all children for read only kind.
    if(parent && !parent->isReadOnly()){
        for(auto node_kind: entity_factory->getNodeKinds()){
            
            auto temp_node = entity_factory->createTempNode(node_kind);
            if(temp_node){
                if(parent->canAdoptChild(temp_node)){
                    kinds << node_kind;
                }
                entity_factory->destructNode(temp_node);
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
 * @brief NewController::displayMessage Called by the Model when a message needs to be visualised
 * Emits controller_DisplayMessage
 * @param title - The title of the message
 * @param message - The message
 * @param ID - The entity ID of which the message relates to.
 */
void ModelController::displayMessage(QString title, QString message, int ID)
{
    Q_UNUSED(title)

    //Emit a signal to the view.
    //NotificationManager::manager()->displayNotification(message, "", "", ID);
}

/**
 * @brief NewController::setData Sets the Value of the Data of an Entity.
 * @param parentID - The ID of the Entity
 * @param keyName - The name of the Key
 * @param dataValue - The new value of the Data.
 */
void ModelController::setData(int parentID, QString keyName, QVariant dataValue)
{
    Entity* graphML = getEntity(parentID);
    if(graphML){
        _setData(graphML, keyName, dataValue, true);
    }
}

void ModelController::removeData(int parentID, QString keyName)
{
    Entity* entity = getEntity(parentID);
    if(entity){
        destructData(entity, keyName, true);
    }
}



/**
 * @brief NewController::importProjects
 * @param xmlDataList
 */
void ModelController::importProjects(QStringList xmlDataList)
{
    lock_.lockForWrite();
    emit showProgress(true, "Importing Projects");
    IMPORTING_PROJECT = true;
    bool success = _importProjects(xmlDataList);
    IMPORTING_PROJECT = false;
    emit showProgress(false);
    lock_.unlock();
    emit controller_ActionFinished(success);
}


/**
 * @brief NewController::importSnippet Imports a Snippet of GraphML into the selection defined by ID provided
 * @param IDs - The list of entity IDs
 * @param fileName - The name of the Snippet imported LABEL.<PARENT_KIND>.snippet
 * @param fileData - The graphml data of the snippet.
 */
void ModelController::importSnippet(QList<int> IDs, QString fileName, QString fileData)
{
    bool success = _importSnippet(IDs, fileName, fileData);
    emit controller_ActionProgressChanged(100);
    emit controller_ActionFinished(success);
}

/**
 * @brief NewController::exportSnippet Exports a Snippet of GraphML based on the selection defined by IDs provided
 * @param IDs - The list of entity IDs
 */
void ModelController::exportSnippet(QList<int> IDs)
{
    QString data = _exportSnippet(IDs);
    emit controller_ExportedSnippet(data);
}

/**
 * @brief NewController::gotQuestionAnswer Got an answer from the View about a question which was asked.
 * @param answer - The Boolean answer to the question
 */
void ModelController::gotQuestionAnswer(bool answer)
{
    questionAnswer = answer;
    emit controller_GotQuestionAnswer();
}


void ModelController::clearUndoHistory()
{
    clearHistory();
}

bool ModelController::askQuestion(QString questionTitle, QString question, int ID)
{
    if(!INITIALIZING){
        //Construct a EventLoop which waits for the View to answer the question.
        QEventLoop waitLoop;
        questionAnswer = false;


        connect(this, SIGNAL(controller_GotQuestionAnswer()), &waitLoop, SLOT(quit()));

        emit controller_AskQuestion(questionTitle, question, ID);

        waitLoop.exec();
        return questionAnswer;
    }
    return false;
}


Node *ModelController::getSingleNode(QList<int> IDs)
{
    foreach(int ID, IDs){
        return getNode(ID);
    }
    return 0;

}

bool ModelController::_newImportGraphML(QString document, Node *parent)
{
    //Lookup for key's ID to Key* object
    QHash <QString, Key*> keyHash;

    //Stacks to store the NodeIDs and EdgeIDs from the document.
    QList<QString> nodeIDStack;
    QList<QString> edgeIDStack;

    //Hash to store the TempEntities constructed from the document.
    QHash <QString, TempEntity*> entityHash;

    //Use the Model as the parent if none provided.
    if(!parent){
        //Set parent as Model item.
        parent = model;
    }

    if(!parent){
        //If we still don't have a parent. Return
        return false;
    }

    if(parent->isInstance() || parent->isImpl()){
        if(!(UNDOING || REDOING)){
            QString message =  "Cannot import/paste into an Instance.";
            //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_WARNING);
            return false;
        }
    }

    if(!isGraphMLValid(document)){
        QString message =  "Invalid GraphML Project";
        //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_WARNING);
        return false;
    }

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);


    bool linkPreviousID = false;
    bool resetPosition = false;

    if((UNDOING || REDOING) || CUT_USED){
        linkPreviousID = true;
    }

    if((PASTE_USED && !CUT_USED) || IMPORTING_WORKERDEFINITIONS){
        resetPosition = true;
    }

    //Construct a top level Entity
    TempEntity* topEntity = new TempEntity(GRAPHML_KIND::NODE);
    topEntity->setActualID(parent->getID());


    TempEntity* currentEntity = topEntity;


    if(updateProgressNotification()){
        emit showProgress(true, "Parsing Project");
        emit progressChanged(-1);
    }


    while(!xml.atEnd()){
        
        //Read each line of the xml document.
        xml.readNext();

        //Get the tagName
        QStringRef tagName = xml.name();
      
        //Process the Tags
        GRAPHML_KIND currentKind = GRAPHML_KIND::NONE;

        if(tagName == "edge"){
            currentKind = GRAPHML_KIND::EDGE;
        }else if(tagName == "node"){
            currentKind = GRAPHML_KIND::NODE;
        }else if(tagName == "data"){
            currentKind = GRAPHML_KIND::DATA;
        }else if(tagName == "key"){
            currentKind = GRAPHML_KIND::KEY;
        }

        if(xml.isStartElement()){
            if(currentKind != GRAPHML_KIND::NONE){
                QString ID = getXMLAttribute(xml, "id");
                int prevID = get_int_from_qstring(ID);


                TempEntity* entity = new TempEntity(currentKind, currentEntity);
                entity->setID(ID);
                entity->setPrevID(prevID);

                entity->setLineNumber(xml.lineNumber());


                if(currentKind == GRAPHML_KIND::EDGE){
                    //Handle Source/Target for edges.
                    QString srcID = getXMLAttribute(xml, "source");
                    QString dstID = getXMLAttribute(xml, "target");

                    entity->setSrcID(srcID);
                    entity->setDstID(dstID);

                    entity->setActualSrcID(get_int_from_qstring(srcID));
                    entity->setActualDstID(get_int_from_qstring(dstID));
                }

                //Insert into the hash.
                entityHash.insert(ID, entity);

                //Add to the correct stack.
                if(currentKind == GRAPHML_KIND::NODE){
                    nodeIDStack.append(ID);
                }else if(currentKind == GRAPHML_KIND::EDGE){
                    edgeIDStack.append(ID);
                }

                //Set the Item as the current Entity.
                currentEntity = entity;

                if(resetPosition && currentEntity && currentEntity->getParentEntity() == topEntity){
                    //Reset the position of the first item, then clear reset position.
                    currentEntity->setResetPosition();
                }
            }else if(currentKind == GRAPHML_KIND::KEY){
                QString ID = getXMLAttribute(xml, "id");

                QString keyName = getXMLAttribute(xml, "attr.name");
                QString keyTypeStr = getXMLAttribute(xml, "attr.type");

                QVariant::Type keyType = Key::getTypeFromGraphML(keyTypeStr);

                Key* key = entity_factory->GetKey(keyName, keyType);
                keyHash.insert(ID, key);
            }else if(currentKind == GRAPHML_KIND::DATA){
                QString keyID = getXMLAttribute(xml, "key");
                Key* key = keyHash[keyID];

                //If we have a key and a current Entity
                if(key && currentEntity){
                    //Attach the data to the current entity.
                    QString dataValue = xml.readElementText();

                    bool addData = true;
                    if(resetPosition && currentEntity && currentEntity->getParentEntity() == topEntity){
                        if(key->getName() == "sortOrder"){
                            addData = false;
                        }
                    }

                    if(addData){
                        Data* data = new Data(key, dataValue);
                        currentEntity->addData(data);
                    }
                }
            }
        }

        if(xml.isEndElement()){
            //For Nodes/Edges, step up a parent.
            if(currentKind != GRAPHML_KIND::NONE){
                currentEntity = currentEntity->getParentEntity();
            }
        }
    }

    //Stores a list of all read only node states which are in the imported documents.
    QList<ReadOnlyState> toImportReadOnlyStates;

    //Stores a list of Node IDs which are Read-Only.
    QList<int> existingReadOnlyIDs;

    //Store a list of readOnlyStates which have been asked about.
    QList<ReadOnlyState> olderSnippetsImported;

    //Deal with read only objects first.
    foreach(QString ID, nodeIDStack){
        TempEntity *entity = entityHash[ID];
        if(entity && entity->gotReadOnlyState()){
            ReadOnlyState state = entity->getReadOnlyState();

            //Check for existance of the read only items.
            if(readOnlyHash.containsKey(state)){
                int nodeID = readOnlyHash.value(state);
                Node* node = getNode(nodeID);
                if(node){
                    ReadOnlyState oldState = getReadOnlyState(node);
                    //If the current state is newer than the historic document, ask the user.
                    if(state.isOlder(oldState)){
                        bool askQ = true;
                        bool importOlder = true;

                        //Check to see if we have asked the question before.
                        foreach(ReadOnlyState oState, olderSnippetsImported){
                            if(state.isSimilar(oState)){
                                askQ = false;
                                importOlder = oState.imported;
                                break;
                            }
                        }

                        if(askQ){
                            importOlder = askQuestion("Import Older Snippet", "You are trying to replace an newer version of a snippet with an older version. Would you like to proceed?", nodeID);
                            state.imported = importOlder;
                            olderSnippetsImported << state;
                        }
                        if(!importOlder){
                            //Ignore construction.
                            entity->setIgnoreConstruction(true);
                            entityHash.remove(ID);
                            continue;
                        }
                    }
                }
                existingReadOnlyIDs << readOnlyHash.value(state);
            }
            toImportReadOnlyStates.append(state);
        }
    }

    
    QList<int> nodeIDsToRemove;

    //Go through each existing read-only nodes and remove the nodes which aren't in the list of toImportReadOnlyStates
    foreach(int ID, existingReadOnlyIDs){
        Node* node = getNode(ID);
        if(node){
            Node* parentNode = node->getParentNode();
            //If the parent Node is a read-only node, go through its children.
            if(parentNode && parentNode->isReadOnly()){
                foreach(Node* child, parentNode->getChildren()){
                    ReadOnlyState state = getReadOnlyState(child);

                    //If we don't have to import this read-only state, we should remove it.
                    if(!toImportReadOnlyStates.contains(state)){
                        int childID = child->getID();
                        nodeIDsToRemove.append(childID);
                    }
                }
            }
        }
    }

    //Remove the items we don't need anymore
    _remove(nodeIDsToRemove, false);

    float totalEntities = entityHash.size();
    float entitiesMade = 0;

    if(updateProgressNotification()){
        emit showProgress(true, "Constructing Nodes");
    }



    
    //Now construct all Nodes.
    while(!nodeIDStack.isEmpty()){
        //Get the String ID of the node.
        QString ID = nodeIDStack.takeFirst();
        
        TempEntity *entity = entityHash[ID];

        if(updateProgressNotification()){
            emit progressChanged((entitiesMade * 100) / totalEntities);
        }

        entitiesMade ++;

        if(entity && entity->isNode() && entity->shouldConstruct()){
            //Check the read only state.
            ReadOnlyState readOnlyState = entity->getReadOnlyState();

            int nodeID = -1;

            //If we have a read-only node, and we have already got a copy of the read-only state
            if(readOnlyState.isValid() && readOnlyHash.containsKey(readOnlyState)){
                //Update the actual ID of the item.
                nodeID = readOnlyHash.value(readOnlyState);

                //Update the data of the node to match what was imported.
                Node* node = getNode(nodeID);
                if(node){
                    //Update Data.
                    //TODO Check to see if all datas were added, if not delete them.
                    _attachData(node, entity->takeDataList());
                }
            }

            //If we haven't seen this node, we need to construct it.
            if(nodeID == -1){
                TempEntity* parentEntity = entity->getParentEntity();

                //If we have a parentEntity and it represents a preconstructed Node
                if(parentEntity && parentEntity->gotActualID()){
                    //Get the parentNode
                    Node* parentNode = getNode(parentEntity->getActualID());
                    
                    NODE_KIND kind = entity_factory->getNodeKind(entity->getKind());
                    Node* newNode = 0;

                        //Don't attach model information for anything but Open
                    if(!OPENING_PROJECT && kind == NODE_KIND::MODEL){
                        newNode = model;
                        //Ignore the construction.
                        entity->setIgnoreConstruction();
                    }else if(kind == NODE_KIND::HARDWARE_NODE || kind == NODE_KIND::HARDWARE_CLUSTER){
                        //If we have a url, use it to check for an existing node/cluster
                        auto url = entity->getData("url").toString();
                        if(url != ""){
                            newNode = get_hardware_by_url(parentNode, kind, url);
                        }
                    }

                    if(!newNode){
                        newNode = construct_node(parentNode, kind);
                    }
                    
                    if(newNode){
                        _attachData(newNode, entity->takeDataList());
                    }else{
                        QString message = "Cannot create Node '" % entity->getKind() % "' from document at line #" % QString::number(entity->getLineNumber()) % ".";
                        //NotificationManager::manager()->displayNotification(message, "", "", parentNode->getID(), NS_WARNING);
                        entity->setIgnoreConstruction();
                        continue;
                    }
                    bool attached = false;

                    if(newNode->isInModel()){
                        attached = true;
                    }else{
                        //Attach the node to the parentNode
                        attached = attachChildNode(parentNode, newNode);
                    }

                    if(attached){
                        nodeID = newNode->getID();
                    }

                    if(linkPreviousID && entity->hasPrevID()){
                        //Link the old ID
                        link_ids(entity->getPrevID(), nodeID);
                    }
                }
            }

            //If we have a valid
            if(readOnlyState.isValid()){
                //Add a lookup into the readOnlyHash for this item.
                if(!readOnlyHash.containsKey(readOnlyState)){
                    readOnlyHash.insert(readOnlyState, nodeID);
                }
            }

            //if we have an ID which isn't yet attached to entity, update the Entity so children nodes can attach to this newly constructed node.
            if(!entity->gotActualID() && nodeID > 0){
                entity->setActualID(nodeID);
            }
        }
    }


    QMultiMap<int, TempEntity*> edgesMap;

    while(!edgeIDStack.isEmpty()){
        //Get the String ID of the node.
        QString ID = edgeIDStack.takeFirst();

        TempEntity* entity = entityHash.value(ID, 0);
        if(entity && entity->isEdge()){
            TempEntity* srcEntity = entityHash.value(entity->getSrcID(), 0);
            TempEntity* dstEntity = entityHash.value(entity->getDstID(), 0);

            Node* src = 0;
            Node* dst = 0;

            if(srcEntity && srcEntity->gotActualID()){
                src = getNode(srcEntity->getActualID());
            }else if(entity->getActualSrcID() > 0){
                src = getNode(entity->getActualSrcID());
            }

            if(dstEntity && dstEntity->gotActualID()){
                dst = getNode(dstEntity->getActualID());
            }else if(entity->getActualDstID() > 0){
                dst = getNode(entity->getActualDstID());
            }

            if(src && dst){
                //Set destination.
                entity->setSource(src);
                entity->setDestination(dst);

                QString kind = entity->getKind();
                EDGE_KIND edgeClass = entity_factory->getEdgeKind(kind);

                //If the edge class stored in the model is invalid we should try all of the edge classes these items can take, in order.
                if(edgeClass == EDGE_KIND::NONE){
                    foreach(EDGE_KIND ec, getPotentialEdgeClasses(src, dst)){
                        entity->appendEdgeKind(ec);
                    }
                }else{
                    entity->appendEdgeKind(edgeClass);
                }

				if(entity->hasEdgeKind()){
                    int index = GetEdgeOrderIndex(entity->getEdgeKind());
					//Insert the item in the lookup
					edgesMap.insertMulti(index, entity);
                }else{
                    QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". NO MORE KINDS.";
                    //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_ERROR);
                }
            }else{
                //Don't construct if we have an error.
				entity->setIgnoreConstruction();
                QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". Missing SRC/DST";
                //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_ERROR);
			}
        }
    }

    if(updateProgressNotification()){
        emit showProgress(true, "Constructing Edges");
    }

    int totalEdges = edgesMap.size();
    int itterateCount = 0;

    while(!edgesMap.isEmpty()){
        QList<TempEntity*> currentEdges;
        QList<TempEntity*> unconstructedEdges;

        int current_kind_key = -1;
        int constructedEdges = 0;

        QList<int> edge_kind_keys = edgesMap.uniqueKeys();
        std::sort(edge_kind_keys.begin(), edge_kind_keys.end());

        //Get all the edges, of kind eK, (Break when we get any edges)
        for(auto key : edge_kind_keys){
            if(edgesMap.contains(key)){
                currentEdges = edgesMap.values(key);
                current_kind_key = key;
                break;
            }
        }

        //If we have edges yet to go, yet we haven't gotten any items in out list to process.
        if(currentEdges.size() == 0 && edgesMap.size() > 0){
            break;
        }

        //Reverse itterate through the list of Entities (QMap inserts in a stack form LIFO)
        for(int i = currentEdges.size() - 1; i >= 0; i --){
            itterateCount ++;
            TempEntity* entity = currentEdges[i];
            entity->incrementRetryCount();

            //Get the edgeKind
            EDGE_KIND edgeKind = entity->getEdgeKind();
            
            //Remove it from the map!
            edgesMap.remove(current_kind_key, entity);

            Node* src = entity->getSource();
            Node* dst = entity->getDestination();

            if(src && dst){
                Edge* edge = src->getEdgeTo(dst, edgeKind);
                if(edge){
                    //Attach the Data to the existing edge.
                    _attachData(edge, entity->takeDataList());
                }else{
                    
                    //Construct an Edge, with the data.
                    edge = constructEdgeWithData(edgeKind, src, dst, entity->takeDataList());
                }

                if(edge){
                    
                    //Link the old ID to the new ID.
                    if(linkPreviousID && entity->hasPrevID()){
                        link_ids(entity->getPrevID(), edge->getID());
                    }

                    //Update the progress.
                    if(updateProgressNotification()){
                        emit progressChanged((entitiesMade * 100) / totalEntities);
                        entitiesMade ++;
                    }
                    constructedEdges ++;
                }else{
                    //Append this item to the list of unconstructed items
                    unconstructedEdges.append(entity);
                }
            }else{
                qCritical() << "Cannot Find Src Dst: " << src << " " << dst;
            }
        }

        //Go through the list of unconstructed edges and do things.
        foreach(TempEntity* entity, unconstructedEdges){
            //If no edges were constructed this pass, we by definition, can't construct any.
            if(constructedEdges == 0){
                //Remove the current edgeKind, so we can try the next (If it has one)
                entity->removeEdgeKind(entity->getEdgeKind());
            }

            if(entity->hasEdgeKind()){
                int new_index = GetEdgeOrderIndex(entity->getEdgeKind());
                //Reinsert back into the map (Goes to the start)
                edgesMap.insertMulti(new_index, entity);
            }else{
                //This entity has no more edge kinds to try, therefore can never be constructed.
                QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ".";
                qCritical() << message;
                //NotificationManager::manager()->displayNotification(message, "", "", -1, NS_ERROR);
            }
        }
    }

    if(totalEdges > 0){
        qCritical() << "Imported: #" << totalEdges << " Edges in " << itterateCount << " Itterations.";
    }

    //Clean up
    foreach(TempEntity* entity, entityHash.values()){
        delete entity;
    }

    entityHash.clear();

    //Clear the topEntity
    delete topEntity;

    return true;
}

ReadOnlyState ModelController::getReadOnlyState(Node *node)
{
    ReadOnlyState state;

    state.snippetID = -1;
    state.snippetMAC = -1;
    state.snippetTime = -1;
    state.exportTime = -1;
    state.isDefinition = false;
    if(node){
        Data* snippetID = node->getData("snippetID");
        Data* snippetMAC = node->getData("snippetMAC");
        Data* snippetTime = node->getData("snippetTime");
        Data* exportTime = node->getData("exportTime");
        Data* readOnlyDefinition = node->getData("readOnlyDefinition");
        if(snippetID){
            state.snippetID = snippetID->getValue().toInt();
        }
        if(snippetMAC){
            state.snippetMAC = snippetMAC->getValue().toLongLong();
        }
        if(snippetTime){
            state.snippetTime = snippetTime->getValue().toInt();
        }
        if(exportTime){
            state.exportTime = exportTime->getValue().toInt();
        }
        if(readOnlyDefinition){
            state.isDefinition = readOnlyDefinition->getValue().toBool();
        }
    }
    return state;
}

EventAction ModelController::getEventAction()
{
    EventAction action;
    action.projectDirty = projectDirty;
    action.Action.ID = currentActionID;
    action.Action.name = currentAction;
    action.Action.actionID = currentActionItemID++;
    return action;
}

void ModelController::setProjectDirty(bool dirty)
{
    if(projectDirty != dirty){
        projectDirty = dirty;
        emit projectModified(dirty);
    }
}

void ModelController::setProjectPath(QString path)
{
    if(projectPath != path){
        projectPath = path;
        emit controller_ProjectFileChanged(projectPath);
    }
}


bool ModelController::canCopy(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canCopy(items);
    lock_.unlock();
    return result;
}

bool ModelController::canReplicate(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canReplicate(items);
    lock_.unlock();
    return result;
}

bool ModelController::canCut(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canCopy(items) && canCopy(items);
    lock_.unlock();
    return result;
}

bool ModelController::canReplicate(QList<Entity *> selection)
{

    if(!canCut(selection)){
        return false;
    }

    Node* parentNode = 0;

    foreach(Entity* item, selection){
        if(item->isNode()){
            parentNode = ((Node*)item)->getParentNode();
            break;
        }
    }

    if(parentNode){
        QList<Entity*> parents;
        parents << parentNode;
        if(canPaste(parents)){
            return true;
        }
    }
    return false;


}

bool ModelController::canCut(QList<Entity *> selection)
{
    return canCopy(selection) && canRemove(selection);
}

bool ModelController::canCopy(QList<Entity *> selection)
{

    bool valid = !selection.isEmpty();
    Node* parent = 0;

    foreach(Entity* item, selection){
        if(item->isNode()){
            Node* node = (Node*) item;
            if(!canDeleteNode(node)){
                valid = false;
                break;
            }
            if(!parent){
                parent = node->getParentNode();
            }else if(node->getParentNode() != parent){
                valid = false;
                break;
            }
        }
    }

    return valid;
}

bool ModelController::canPaste(QList<Entity *> selection)
{
    if(selection.size() == 1){
        Entity* item = selection.first();

        if(item && item->isNode() && !item->isReadOnly() && item != model){
            Node* node = (Node*) item;
            if(!node->isInstance() && !node->isImpl()){
                return true;
            }
        }
    }
    return false;
}

bool ModelController::canRemove(QList<Entity *> selection)
{
    if(selection.isEmpty()){
        return false;
    }

    foreach(Entity* entity, selection){
        if(entity->isNode()){
            Node* node = (Node*) entity;
            Node* parentNode = node->getParentNode();

            if(!canDeleteNode(node)){
                return false;
            }
            if(parentNode){
                switch(node->getNodeKind()){
                    case NODE_KIND::VARIABLE:{
                        if(node->isInstance()){
                            return false;
                        }
                        break;
                    }
                    case NODE_KIND::INPUT_PARAMETER:{
                        if(!node->getDataValue("is_variadic").toBool()){
                            return false;
                        }
                        break;
                    }
                    case NODE_KIND::RETURN_PARAMETER:{
                        return false;
                        break;
                    }
                    default:
                        break;
                }

                if(node->isImpl() && node->getDefinition()){
                    //Only allowed to delete OutEventPortImpls
                    if(node->getNodeKind() != NODE_KIND::OUTEVENTPORT_IMPL){
                        return false;
                    }
                }

                if(node->isInstance() && parentNode->isInstance()){
                    return false;
                }

                if(node->isReadOnly() && parentNode->isReadOnly()){
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

bool ModelController::canSetReadOnly(QList<Entity *> selection)
{
    foreach(Entity* item, selection){
        if(!item->isReadOnly()){
            return true;
        }
    }
    return false;
}

bool ModelController::canUnsetReadOnly(QList<Entity *> selection)
{
    foreach(Entity* item, selection){
        if(item->isReadOnly()){
            return true;
        }
    }
    return false;
}

bool ModelController::canExportSnippet(QList<Entity *> selection)
{
    if(selection.isEmpty()){
        return false;
    }

    Node* parent = 0;
    foreach(Entity* item, selection){
        if(item->isNode()){
            Node* node = (Node*) item;

            if(nonSnippetableKinds.contains(node->getNodeKind())){
                return false;
            }

            if(node->gotData("readOnly") && node->getDataValue("readOnly").toBool()){
                //Can't Export Read-Only Stuffs.
                return false;
            }

            if(!parent){
                parent = node->getParentNode();

                if(!parent || !snippetableParentKinds.contains(parent->getNodeKind())){
                    return false;
                }
            }else{
                if(node->getParentNode() != parent){
                    //Must share parents.
                    return false;
                }
            }
        }
    }
    return true;
}

bool ModelController::canImportSnippet(QList<Entity *> selection)
{
    if(selection.length() == 1){
        Entity* item = selection.at(0);
        if(item->isNode()){
            Node* node = (Node*) item;
            if(snippetableParentKinds.contains(node->getNodeKind())){
                return true;
            }
        }
    }
    return false;
}

bool ModelController::canRemove(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canRemove(items);
    lock_.unlock();
    return result;
}

bool ModelController::canPaste(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canPaste(items);
    lock_.unlock();
    return result;
}

bool ModelController::canExportSnippet(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canExportSnippet(items);
    lock_.unlock();
    return result;
}

bool ModelController::canImportSnippet(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canImportSnippet(items);
    lock_.unlock();
    return result;
}

bool ModelController::canSetReadOnly(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canSetReadOnly(items);
    lock_.unlock();
    return result;
}

bool ModelController::canUnsetReadOnly(QList<int> selection)
{
    lock_.lockForRead();
    QList<Entity*> items = getOrderedSelection(selection);
    bool result = canUnsetReadOnly(items);
    lock_.unlock();
    return result;
}

bool ModelController::canUndo()
{
    return !undoActionStack.isEmpty();
}

bool ModelController::canRedo()
{

    return !redoActionStack.isEmpty();
}

/**
 * @brief NewController::canLocalDeploy Checks to see if Model only contains deployment edges to the local node.
 * @return
 */
bool ModelController::canLocalDeploy()
{
    lock_.lockForRead();

    bool result = true;
    if(assemblyDefinitions){
        int count = 0;
        //Check to see if all nodes in the assembly definitions are deployed to the localhost node.
        foreach(Edge* edge, assemblyDefinitions->getEdges(-1, EDGE_KIND::DEPLOYMENT)){
            if(edge->isConnected(localhostNode)){
                count ++;
            }else{
                count = 0;
                break;
            }
        }
        result = count > 0;
    }
    lock_.unlock();
    return result;
}

QString ModelController::getProjectPath() const
{
    return projectPath;
}



bool ModelController::isProjectSaved() const
{
    return projectDirty;
}


int ModelController::getDefinition(int ID)
{
    lock_.lockForRead();
    Node* original = getNode(ID);
    int defID = -1;
    if(original){
        Node* node = original->getDefinition(true);
        if(node && node != original){
            defID = node->getID();
        }
    }
    lock_.unlock();
    return defID;
}

int ModelController::getImplementation(int ID)
{
    lock_.lockForRead();
    int implID = -1;
    Node* original = getNode(ID);
    if(original){
        int definitionID = getDefinition(ID);
        if(definitionID == -1){
            definitionID = ID;
        }
        Node* node = getNode(definitionID);

        if (node && node->getImplementations().size() > 0) {
            Node* impl = node->getImplementations().at(0);
            if (impl != original) {
                implID = impl->getID();
            }
        }
    }
    lock_.unlock();
    return implID;
}

Node* ModelController::getSharedParent(QList<int> IDs)
{
    Node* parent = 0;

    foreach(int ID, IDs){
        Node* node = getNode(ID);
        if(!node){
            continue;
        }
        if(!parent){
            //Set the firstParent to the first Nodes parent.
            parent = node->getParentNode();
        }else if(node->getParentNode() != parent){
            return 0;
        }
    }
    return parent;
}



QString ModelController::getDataValueFromKeyName(QList<Data *> dataList, QString keyName)
{
    foreach(Data* data, dataList){
        if(data && data->getKeyName() == keyName){
            return data->getValue().toString();
        }
    }
    return "";
}

void ModelController::setDataValueFromKeyName(QList<Data *> dataList, QString keyName, QString value)
{
    foreach(Data* data, dataList){
        if(data && data->getKeyName() == keyName){
            data->setValue(value);
            return;
        }
    }
}

/**
 * @brief NewController::getProcessName Gets the long name used to store Process definitions in the workerProcesses map.
 * @param process The Process
 * @return worker_operation
 */
QString ModelController::getWorkerProcessName(WorkerProcess *process)
{
    if(process){
        QString workerName = process->getDataValue("worker").toString();
        QString operationName = process->getDataValue("operation").toString();
        if(workerName != "" && operationName != ""){
            return workerName + "_" + operationName;
        }

    }
    return "";
}


bool ModelController::isUserAction()
{
    if(UNDOING || REDOING || OPENING_PROJECT || IMPORTING_PROJECT || INITIALIZING || PASTE_USED || DESTRUCTING_CONTROLLER){
        return false;
    }else{
        return true;
    }
}


Node* ModelController::get_hardware_by_url(Node* parent_node, NODE_KIND kind, QString url){
    if(parent_node){
        for(auto node : parent_node->getChildrenOfKind(kind)){
            auto node_url = node->getDataValue("url");
            if(node_url == url){
                return node;
            }
        }
    }
    return 0;
}
