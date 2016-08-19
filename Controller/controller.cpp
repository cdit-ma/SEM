#include "controller.h"
#include "../View/nodeview.h"
#include <QDebug>
#include <algorithm>
#include <QDateTime>
#include <QMessageBox>
#include <QEventLoop>
#include <QObject>
#include <QSysInfo>
#include <QDir>
#include <QStringBuilder>
#include <QReadWriteLock>
#include "edgeadapter.h"
#include "nodeadapter.h"
#include "../Model/tempentity.h"

bool UNDO = true;
bool REDO = false;
bool SETUP_AS_INSTANCE = true;
bool SETUP_AS_IMPL = false;

static int count = 0;

NewController::NewController() :QObject(0)
{

    qCritical() << count ++;
    controllerThread = new QThread(this);
    moveToThread(controllerThread);

    connect(this, SIGNAL(destroyed(QObject*)), controllerThread, SLOT(terminate()));
    connect(this, &NewController::initiateTeardown, this, &QObject::deleteLater, Qt::QueuedConnection);
    controllerThread->start();


    qRegisterMetaType<ENTITY_KIND>("ENTITY_KIND");
    qRegisterMetaType<MESSAGE_TYPE>("MESSAGE_TYPE");
    qRegisterMetaType<GraphML::GRAPHML_KIND>("GraphML::GRAPHML_KIND");
    qRegisterMetaType<Edge::EDGE_CLASS>("Edge::EDGE_CLASS");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<ReadOnlyState>("ReadOnlyState");

    Model::resetID();


    logFile = 0;

    PASTE_USED = false;
    REPLICATE_USED = false;
    CONSTRUCTING_WORKERFUNCTION = false;
    IMPORTING_PROJECT = false;
    USE_LOGGING = false;
    UNDOING = false;
    OPENING_PROJECT = false;
    IMPORTING_WORKERDEFINITIONS = false;
    INITIALIZING = true;
    viewSignalsEnabled = true;
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


    viewAspects << "Assembly" << "Workload" << "Definitions" << "Hardware";
    protectedKeyNames << "kind" << "localhost";
    protectedKeyNames << "description";
    //protectedKeyNames << "worker" << "operation" << "workerID" << "description";
    protectedKeyNames << "snippetMAC" << "snippetTime" << "snippetID" << "exportTime" << "readOnlyDefinition";

    visualKeyNames << "x" << "y" << "width" << "height" << "isExpanded" << "readOnly" << "dataProtected";


    containerNodeKinds << "Model";
    containerNodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    containerNodeKinds << "HardwareDefinitions" << "AssemblyDefinitions" << "ManagementComponent";
    containerNodeKinds << "HardwareCluster";


    definitionNodeKinds << "IDL" << "Component" << "Attribute" << "ComponentAssembly" << "ComponentInstance" << "BlackBox" << "BlackBoxInstance";
    definitionNodeKinds << "Member" << "Aggregate";
    definitionNodeKinds << "InEventPort"  << "OutEventPort";
    definitionNodeKinds << "InEventPortDelegate"  << "OutEventPortDelegate";
    definitionNodeKinds << "AggregateInstance";
    definitionNodeKinds << "ComponentImpl";

    definitionNodeKinds << "Vector" << "VectorInstance";


    definitionNodeKinds << "DDS_QOSProfile";

    dds_QosNodeKinds << "DDS_DeadlineQosPolicy";
    dds_QosNodeKinds << "DDS_DestinationOrderQosPolicy";
    dds_QosNodeKinds << "DDS_DurabilityQosPolicy";
    dds_QosNodeKinds << "DDS_DurabilityServiceQosPolicy";
    dds_QosNodeKinds << "DDS_EntityFactoryQosPolicy";
    dds_QosNodeKinds << "DDS_GroupDataQosPolicy";
    dds_QosNodeKinds << "DDS_HistoryQosPolicy";
    dds_QosNodeKinds << "DDS_LatencyBudgetQosPolicy";
    dds_QosNodeKinds << "DDS_LifespanQosPolicy";
    dds_QosNodeKinds << "DDS_LivelinessQosPolicy";
    dds_QosNodeKinds << "DDS_OwnershipQosPolicy";
    dds_QosNodeKinds << "DDS_OwnershipStrengthQosPolicy";
    dds_QosNodeKinds << "DDS_PartitionQosPolicy";
    dds_QosNodeKinds << "DDS_PresentationQosPolicy";
    dds_QosNodeKinds << "DDS_ReaderDataLifecycleQosPolicy";
    dds_QosNodeKinds << "DDS_ReliabilityQosPolicy";
    dds_QosNodeKinds << "DDS_ResourceLimitsQosPolicy";
    dds_QosNodeKinds << "DDS_TimeBasedFilterQosPolicy";
    dds_QosNodeKinds << "DDS_TopicDataQosPolicy";
    dds_QosNodeKinds << "DDS_TransportPriorityQosPolicy";
    dds_QosNodeKinds << "DDS_UserDataQosPolicy";
    dds_QosNodeKinds << "DDS_WriterDataLifecycleQosPolicy";







    behaviourNodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload" << "OutEventPortImpl";
    behaviourNodeKinds << "WhileLoop" << "InputParameter" << "ReturnParameter" << "AggregateInstance" << "VectorInstance" << "WorkerProcess";


    //Append Kinds which can't be constructed by the GUI.
    constructableNodeKinds << "MemberInstance" << "AttributeImpl";
    constructableNodeKinds << "OutEventPortInstance" << "MemberInstance" << "AggregateInstance";
    constructableNodeKinds << "AttributeInstance" << "AttributeImpl";
    constructableNodeKinds << "InEventPortInstance" << "InEventPortImpl";
    constructableNodeKinds << "OutEventPortInstance" << "OutEventPortImpl" << "HardwareNode";
    constructableNodeKinds << "QOSProfile";

    snippetableParentKinds << "ComponentImpl" << "InterfaceDefinitions";
    nonSnippetableKinds << "OutEventPortImpl" << "InEventPortImpl";

    constructableNodeKinds.append(definitionNodeKinds);
    constructableNodeKinds.append(behaviourNodeKinds);
    constructableNodeKinds.append(dds_QosNodeKinds);
    constructableNodeKinds << "ManagementComponent";

    constructableNodeKinds << "InputParameter";

    constructableNodeKinds.removeDuplicates();

    guiConstructableNodeKinds.append(definitionNodeKinds);
    guiConstructableNodeKinds.append(behaviourNodeKinds);

    guiConstructableNodeKinds.append(dds_QosNodeKinds);

    guiConstructableNodeKinds.removeDuplicates();
    guiConstructableNodeKinds.sort();





}
void NewController::connectViewController(ViewController *view)
{
    connect(this, &NewController::entityConstructed, view, &ViewController::controller_entityConstructed);
    connect(this, &NewController::entityDestructed, view, &ViewController::controller_entityDestructed);
    connect(view, &ViewController::vc_setupModel, this, &NewController::setupController);
    connect(this, &NewController::controller_IsModelReady, view, &ViewController::setControllerReady);

    connect(this, &NewController::dataChanged, view, &ViewController::controller_dataChanged);
    connect(this, &NewController::dataRemoved, view, &ViewController::controller_dataRemoved);

    connect(this, &NewController::propertyChanged, view, &ViewController::controller_propertyChanged);
    connect(this, &NewController::propertyRemoved, view, &ViewController::controller_propertyRemoved);

    connect(view, &ViewController::vc_importProjects, this, &NewController::importProjects);
    connect(view, &ViewController::vc_openProject, this, &NewController::openProject);


    connect(view, &ViewController::vc_setData, this, &NewController::setData);
    connect(view, &ViewController::vc_removeData, this, &NewController::removeData);

    connect(view, &ViewController::vc_constructNode, this, &NewController::constructNode);
    connect(view, &ViewController::vc_constructConnectedNode, this, &NewController::constructConnectedNode);


    connect(view, &ViewController::vc_projectSaved, this, &NewController::setProjectSaved);




    connect(view, &ViewController::vc_undo, this, &NewController::undo);
    connect(view, &ViewController::vc_redo, this, &NewController::redo);
    connect(view, &ViewController::vc_triggerAction, this, &NewController::triggerAction);



    connect(view, &ViewController::vc_cutEntities, this, &NewController::cut);
    connect(view, &ViewController::vc_copyEntities, this, &NewController::copy);
    connect(view, &ViewController::vc_paste, this, &NewController::paste);
    connect(view, &ViewController::vc_replicateEntities, this, &NewController::replicate);
    connect(view, &ViewController::vc_deleteEntities, this, &NewController::remove);




    connect(this, &NewController::projectModified, view, &ViewController::mc_projectModified);
    connect(this, &NewController::controller_ProjectFileChanged, view, &ViewController::vc_projectPathChanged);


    connect(this, &NewController::controller_IsModelReady, view, &ViewController::setModelReady);

    connect(this, &NewController::controller_SetClipboardBuffer, view, &ViewController::setClipboardData);
    connect(this, &NewController::controller_ActionFinished, view, &ViewController::actionFinished);

    connect(this, &NewController::undoRedoChanged, view, &ViewController::mc_undoRedoUpdated);

    connect(this, &NewController::showProgress, view, &ViewController::mc_showProgress);
    connect(this, &NewController::progressChanged, view, &ViewController::mc_progressChanged);


    view->setController(this);
}

void NewController::disconnectViewController(ViewController *view)
{
    view->disconnect(this);
    this->disconnect(view);
    emit initiateTeardown();
}



void NewController::setupController()
{
    lock.lockForWrite();
    setupModel();
    loadWorkerDefinitions();
    clearHistory();
    lock.unlock();
    INITIALIZING = false;

    emit controller_IsModelReady(true);

    emit controller_ModelReady();

    emit controller_ProjectFileChanged("Untitled Project");
    emit projectModified(true);
}

NewController::~NewController()
{
    enableDebugLogging(false);

    DESTRUCTING_CONTROLLER = true;

    destructNode(workerDefinitions);
    destructNode(model);


    while(!keys.isEmpty()){
        delete keys.take(keys.keys().first());
    }
}

void NewController::setExternalWorkerDefinitionPath(QString path)
{
    this->externalWorkerDefPath = path;
}

/**
 * @brief NewController::loadWorkerDefinitions Loads in both the compiled in WorkerDefinitions and Externally defined worker defintions.
 */
void NewController::loadWorkerDefinitions()
{
    //We will be importing into the workerDefinitions aspect.
    Node* workerDefinition = getWorkerDefinitions();

    if(workerDefinition){
        QList<QDir> workerDirectories;
        workerDirectories << QDir(":/WorkerDefinitions");
        if(externalWorkerDefPath != ""){
            workerDirectories << QDir(externalWorkerDefPath);
        }


        QStringList filesToLoad;
        QStringList fileExtension("*.worker.graphml");

        foreach(QDir directory, workerDirectories){
            //Foreach *.worker.graphml file in the workerDefPath, load the graphml.
            foreach(QString fileName, directory.entryList(fileExtension)){
                if(fileName == "VariableOperation.worker.graphml"){
                    //Ignore VariableOperation
                    continue;
                }

                QString importFileName = directory.absolutePath() + "/" + fileName;
                filesToLoad << importFileName;
            }
        }

        IMPORTING_WORKERDEFINITIONS = true;
        foreach(QString file, filesToLoad){
            QPair<bool, QString> data = readFile(file);
            //If the file was read.
            if(data.first){
                bool success = _newImportGraphML(data.second, workerDefinition);

                if(!success){
                    QString message = "Cannot import worker definition '" + file +"'";
                    emit controller_DisplayMessage(WARNING, message, "Worker Definition Error", "Warning");
                }else{
                    //qCritical() << "Loaded Worker Definition: " << file;
                }
            }else{
                QString message = "Cannot read worker definition '" + file +"'";
                emit controller_DisplayMessage(WARNING, message, "Worker Definition Error", "Warning");
            }
        }


        foreach(Node* child, workerDefinition->getChildren()){
            Process* process = dynamic_cast<Process*>(child);
            QString longName = getProcessName(process);
            if(longName != ""){
                if(!workerProcesses.contains(longName)){
                    workerProcesses[longName] = process;
                }else{
                    QString message = "2 Worker Operations with the same name, Using only the first.";
                    emit controller_DisplayMessage(WARNING, message, "Worker Definition Error", "Warning");
                }
            }
        }
        IMPORTING_WORKERDEFINITIONS = false;
    }
    //Once we have loaded in workers, we should keep a dictionary lookup for them.
}

QString NewController::_exportGraphMLDocument(QList<int> nodeIDs, bool allEdges, bool GUI_USED, bool ignoreVisuals)
{
    bool exportAllEdges = allEdges;

    QString keyXML, edgeXML, nodeXML;
    QList<Node*> containedNodes;
    QList<Key*> containedKeys;
    QList<Edge*> containedEdges;


    //Get all Children and Edges.
    foreach(int ID, nodeIDs){
        Node* node = getNodeFromID(ID);
        if(node){
            if(containedNodes.contains(node) == false){
                containedNodes.append(node);
            }


            //Get all keys used by this node.
            foreach(Key* key, node->getKeys())
            {
                //Add the <key> tag to the list of Keys contained.
                if(!containedKeys.contains(key)){
                    containedKeys.append(key);
                    keyXML += key->toGraphML(1);
                }
            }

            //Get all Children in this node.
            foreach(Node* childNode, node->getChildren()){
                if(childNode && (containedNodes.contains(childNode) == false)){
                    containedNodes.append(childNode);
                }
            }
        }
    }


    bool copySelectionQuestion = false;
    foreach(int ID, nodeIDs){
        Node* node = getNodeFromID(ID);
        if(!node){
            continue;
        }
        foreach(Edge* edge, node->getEdges()){
            Node* src = edge->getSource();
            Node* dst = edge->getDestination();

            //If the source and destination for all edges are inside the selection, then copy it.
            bool containsSrc = containedNodes.contains(src);
            bool containsDst = containedNodes.contains(dst);
            bool exportEdge = false;


            if(containsSrc && containsDst){
                exportEdge = true;
            }else{
                //One or the other.
                if(edge->isAssemblyLevelLink()){
                    exportEdge = true;
                }else if(edge->isAggregateLink() || edge->isInstanceLink()){
                    exportEdge = true;
                }else if(edge->isImplLink()){
                    exportEdge = false;
                }else{
                    if(GUI_USED && !copySelectionQuestion){
                        exportAllEdges = askQuestion(CRITICAL, "Copy Selection?", "The current selection contains edges that are not fully encapsulated. Would you like to copy these edges?", src->getID());
                        copySelectionQuestion = true;
                        GUI_USED = false;
                    }
                    if(exportAllEdges){
                        exportEdge = true;
                    }
                }
            }

            if(exportEdge && !containedEdges.contains(edge)){
                containedEdges.append(edge);
                edgeXML += edge->toGraphML(2);

                //Get the Keys related to this edge.
                foreach(Key* key, edge->getKeys()){
                    if(!containedKeys.contains(key)){
                        containedKeys.append(key);
                        keyXML += key->toGraphML(1);
                    }
                }
            }

        }
        //Export the XML for this node
        if(!ignoreVisuals){
            nodeXML += node->toGraphML(2);
        }else{
            nodeXML += node->toGraphMLNoVisualData(2);
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

QString NewController::_exportGraphMLDocument(Node *node, bool allEdges, bool GUI_USED)
{
    QList<int> nodeIDs;
    nodeIDs << node->getID();
    return _exportGraphMLDocument(nodeIDs, allEdges, GUI_USED);
}


bool NewController::_clear()
{
    bool reply = askQuestion(CRITICAL, "Clear Model?", "Are you sure you want to clear the model? You cannot undo this action.");
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



void NewController::_setData(Entity *parent, QString keyName, QVariant dataValue, bool addAction)
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
    action.Action.kind = GraphML::GK_DATA;
    action.Data.keyName = keyName;

    Data* data = parent->getData(keyName);

    Node* node = (Node*) parent;
    if(parent->hasData(keyName)){
        action.ID = parent->getID();
        action.Data.oldValue = parent->getDataValue(keyName);

        if(dataValue == action.Data.oldValue){
            //Don't update if we have got the same value in the model.
            return;
        }

        bool need2Set = true;
        //If we are dealing with a Node
        if(parent->isNode()){
            if(keyName == "label"){
                enforceUniqueLabel(node, dataValue.toString());
                need2Set = false;
            }else if(keyName == "sortOrder" && node->getParentNode()){
                enforceUniqueSortOrder(node, dataValue.toInt());
                need2Set = false;
            }
        }

        if(need2Set){
            parent->setDataValue(keyName, dataValue);
        }

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
void NewController::attachData(Entity *parent, Data *data, bool addAction)
{
    if(!parent || !data){
        qCritical() << "attachData() parent or data is NULL!";
        return;
    }

    //Construct an Action to reverse the update
    EventAction action = getEventAction();
    action.ID = data->getID();
    action.parentID = parent->getID();
    action.Action.type = CONSTRUCTED;
    action.Action.kind = GraphML::GK_DATA;
    action.Data.keyName = data->getKeyName();
    action.Data.oldValue = data->getValue();

    //Attach the Data to the parent
    parent->addData(data);

    action.Data.newValue = parent->getDataValue(action.Data.keyName);


    //Add an action to the stack.
    addActionToStack(action, addAction);
}

/**
 * @brief NewController::destructData - Removes and destroys a Data attached to an Entity
 * @param parent - The Entity the Data is attached to.
 * @param keyName - The Name of the Key of the Data
 * @param addAction - Add an undo state
 */
bool NewController::destructData(Entity *parent, QString keyName, bool addAction)
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
    action.Action.kind = GraphML::GK_DATA;
    action.Data.keyName = keyName;
    action.Data.oldValue = data->getValue();

    //Remove the Data to the parent
    parent->removeData(data);
    delete data;

    //Add an action to the stack.
    addActionToStack(action, addAction);
    return true;
}

void NewController::setViewSignalsEnabled(bool enabled, bool sendQueuedSignals)
{
    if(enabled == viewSignalsEnabled){
        //Ignore multisetting.
        return;
    }

    if(enabled){
        if(sendQueuedSignals){
            //Send the queued signals.
            while(!viewSignalsList.isEmpty()){
                ViewSignal signal = viewSignalsList.takeFirst();
                if(signal.constructSignal){
                    Entity* item = getGraphMLFromID(signal.id);
                    if(item){
                        emit controller_GraphMLConstructed(item);
                    }
                }else{
                    emit controller_GraphMLDestructed(signal.id, signal.kind);
                }
            }
        }
    }else{
        //Clear the queued signals.
        viewSignalsList.clear();
    }

    viewSignalsEnabled = enabled;
}

void NewController::updateUndoRedoState()
{
    int undos = undoActionStack.size();
    int redos = undoActionStack.size();

    if(undos <= 1 || redos <= 1){
        emit undoRedoChanged();
    }
    if(undoActionStack.isEmpty()){

        emit controller_CanUndo(false);
    }else if(undoActionStack.size() == 1){
        emit controller_CanUndo(true);
    }

    if(redoActionStack.isEmpty()){
        emit controller_CanRedo(false);
    }else if(redoActionStack.size() == 1){
        emit controller_CanRedo(true);
    }
}


void NewController::constructNode(int parentID, QString kind, QPointF centerPoint)
{
    qCritical() << parentID;
    qCritical() << kind;
    if(kind != ""){
        bool ignore = false;
        Node* parentNode = getNodeFromID(parentID);

        //Return if we are read only
        if(parentNode->isReadOnly()){
            qCritical() << parentNode->toString() << " IS READ ONLY";
            emit controller_ActionFinished();
            return;
        }

        QList<Data*> data;
        //Only allow construction of
        if(kind == "InputParameter"){
            Node* matchingParameter = 0;

            QList<Node*> inputParameters = parentNode->getChildrenOfKind("InputParameter", 0);


            //Look for matching variables.
            foreach(Node* child, inputParameters){
                QString label = child->getDataValue("label").toString();
                QString type = child->getDataValue("type").toString();
                if(label == "parameter" && type == "WE_UTE_VariableArguments"){
                    //Clone the data.
                    matchingParameter = child;
                    break;
                }
            }
            if(matchingParameter){
                data = cloneNodesData(matchingParameter);
                data.append(constructPositionDataVector(centerPoint));
                foreach(Data* d, data){
                    if(d->getKeyName() == "value"){
                        d->setValue("");
                        break;
                    }
                }
            }else{
                //If we don't have a matching parameter.
                ignore = true;
            }
        }else if(kind == "Process"){
            data = constructDataVector(kind, centerPoint);

        }else if(kind == "DDS_QOSProfile"){
            ignore = true;
            constructDDSQOSProfile(parentID, centerPoint);
        }else{
            data = constructDataVector(kind, centerPoint);
        }
        if(!ignore){
            triggerAction("Constructing Child Node");
            constructChildNode(parentNode, data);
        }
    }
    emit controller_ActionFinished();
}

void NewController::constructEdge(int srcID, int dstID, Edge::EDGE_CLASS edgeClass)
{
    lock.lockForWrite();
    Node* src = getNodeFromID(srcID);
    Node* dst = getNodeFromID(dstID);

    bool success = false;
    if(src && dst){
        Edge* edge = 0;

        edge = _constructEdge(src, dst);
        success = edge;
    }
    lock.unlock();

    emit controller_ActionFinished(success, "Edge couldn't be constructed");
}

void NewController::destructEdge(int srcID, int dstID, Edge::EDGE_CLASS edgeClass)
{
    lock.lockForWrite();
    Node* src = getNodeFromID(srcID);
    Node* dst = getNodeFromID(dstID);

    bool success = false;
    if(src && dst){
        Edge* edge = src->getEdgeTo(dst);

        if(edge && edge->getEdgeClass() == edgeClass){
            success = destructEdge(edge);
        }
    }
    lock.unlock();

    emit controller_ActionFinished(success, "Edge couldn't be destructed");
}

void NewController::constructWorkerProcessNode(int parentID, QString workerName, QString operationName, QPointF position)
{
    Node* parentNode = getNodeFromID(parentID);

    Process* processDefinition = getWorkerProcess(workerName, operationName);


    triggerAction("Constructing worker Process");
    CONSTRUCTING_WORKERFUNCTION = true;
    Node* processFunction = cloneNode(processDefinition, parentNode);
    CONSTRUCTING_WORKERFUNCTION = false;

    if(processFunction){
        processFunction->setDataValue("x", position.x());
        processFunction->setDataValue("y", position.y());
        if(!(workerName == "WE_UTE" && (operationName == "cppCode" || operationName == "cppHeader"))){
            Key* protectedKey = constructKey("dataProtected", QVariant::Bool, Entity::EK_NODE);
            attachData(processFunction, new Data(protectedKey, true));
        }
    }

    emit controller_ActionFinished();
    return;
}

void NewController::constructDDSQOSProfile(int parentID, QPointF position)
{
    Node* parentNode = getNodeFromID(parentID);

    if(parentNode){
        triggerAction("Constructing DDS QOS Profile");
        Node* profile = constructChildNode(parentNode, constructDataVector("DDS_QOSProfile", position));
        if(profile){
            foreach(QString kind, dds_QosNodeKinds){
                Node* child = constructChildNode(profile, constructDataVector(kind));
            }
        }
    }
}


void NewController::constructConnectedNode(int parentID, QString kind, QPointF centerPoint, int connectedID)
{
    Node* parentNode = getNodeFromID(parentID);
    Node* connectedNode = getNodeFromID(connectedID);
    if(parentNode && connectedNode){
        //Disable the auto send of construct graphml items.
        setViewSignalsEnabled(false);
        triggerAction("Constructed Connected Node");

        //Create a test node, without telling the GUI.
        Node* testNode = _constructNode(constructDataVector(kind));
        if(testNode){
            if(parentNode->canAdoptChild(testNode)){
                parentNode->addChild(testNode);
                //if we can create an edge to this test node, remove it and recreate a new Node properly
                bool edgeOkay = testNode->canConnect(connectedNode);
                //Remove it.
                delete testNode;

                if(edgeOkay){
                    Node* newNode = constructChildNode(parentNode, constructDataVector(kind, centerPoint));
                    if(newNode){
                        //Update the position

                        //Attach but don't send a GUI request.
                        attachChildNode(parentNode, newNode);

                        //Constrct an Edge between the 2 items.
                        constructEdgeWithData(newNode, connectedNode);

                        //Try the alternate connection.
                        if(!newNode->gotEdgeTo(connectedNode)){
                            constructEdgeWithData(connectedNode, newNode);
                        }


                        if(!newNode->getEdgeTo(connectedNode)){
                            qCritical() << "MEGA ERROR";
                        }
                    }
                }else{
                    emit controller_DisplayMessage(WARNING, "Cannot construct edge; Cycle detected.", "Construction Error", "ConnectTo");
                }
            }else{
                delete testNode;
                emit controller_DisplayMessage(WARNING, "Parent cannot adopt entity '" + kind +"'", "Construction Error", "Cancel");
            }

        }
        setViewSignalsEnabled(true);
        //If we can't connect destruct the node we created.
    }
    emit controller_ActionFinished();
}



/**
 * @brief NewController::constructDestructEdges
 * @param srcIDs
 * @param dstID
 * @param construct
 */
void NewController::constructDestructEdges(QList<int> destruct_srcIDs, QList<int> destruct_dstIDs, QList<int> construct_srcIDs, int dstID)
{
    if (destruct_srcIDs.count() == destruct_dstIDs.count()) {
        for (int i = 0; i < destruct_srcIDs.count(); i++) {
            Node* src = getNodeFromID(destruct_srcIDs[i]);
            Node* dst = getNodeFromID(destruct_dstIDs[i]);
            if (src && dst) {
                destructEdge(src->getEdgeTo(dst));
            }
        }
    }

    foreach (int srcID, construct_srcIDs) {
        Node* src = getNodeFromID(srcID);
        Node* dst = getNodeFromID(dstID);
        constructEdgeWithData(src, dst);
    }

    emit controller_ActionFinished();
}


Edge* NewController::constructEdgeWithData(Node *src, Node *dst, QList<Data *> data, int previousID)
{
    Edge* edge = _constructEdge(src, dst);
    if(edge){
        _attachData(edge, data, false);

        //If we are Undo-ing or Redo-ing and we have an ID to link it to.
        if((UNDOING || REDOING) && previousID != -1){
            linkOldIDToID(previousID, edge->getID());
        }
        constructEdgeGUI(edge);
    }
    return edge;
}

QList<Node *> NewController::getAllNodes()
{
    return getNodes(nodeIDs);
}

QList<Node *> NewController::getNodes(QList<int> IDs)
{
    QList<Node* > nodes;
    foreach(int ID, IDs){
        Node* node = getNodeFromID(ID);
        if(node){
            nodes.append(node);
        }
    }
    return nodes;

}


void NewController::triggerAction(QString actionName)
{
    actionCount++;
    currentAction = actionName;
    currentActionID = actionCount;
    updateUndoRedoState();
}

void NewController::undo()
{
    if(canUndo()){
        emit showProgress(true, "Undoing");
        undoRedo(UNDO, true);
        emit showProgress(false);
    }
    emit controller_ActionFinished();
}

void NewController::redo()
{
    if(canRedo()){
        emit showProgress(true, "Redoing");
        undoRedo(REDO, true);
        emit showProgress(false, "Redoing");
    }
    emit controller_ActionFinished();
}

void NewController::openProject(QString filePath, QString xmlData)
{
    lock.lockForWrite();
    OPENING_PROJECT = true;
    bool result = _newImportGraphML(xmlData, getModel());
    OPENING_PROJECT = false;
    lock.unlock();

    //Update the project filePath
    setProjectPath(filePath);
    //Clear the Undo/Redo History.
    clearHistory();
    //Loading a project means we are in state with the savefile.
    setProjectDirty(false);

    //Hide the progress bar.
    emit showProgress(false);

    emit controller_ActionFinished(result, "Project couldn't be opened.");
}


/**
 * @brief NewController::copy - Attempts to copy a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void NewController::copy(QList<int> IDs)
{

    lock.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canCopy(selection)){
        QString value = _copy(selection);

        if(!value.isEmpty()){
            emit controller_SetClipboardBuffer(value);
            success = true;
        }
    }

    lock.unlock();
    emit controller_ActionFinished(success, "Cannot copy selection.");
}

/**
 * @brief NewController::remove - Attempts to remove a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void NewController::remove(QList<int> IDs)
{
    lock.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    if(canDelete(selection)){
        emit triggerAction("Removing Selection");
        bool success = _remove(selection);
        emit controller_ActionFinished(success, "Cannot delete all selected entities.");
    } else {
        emit controller_ActionFinished();
    }
    lock.unlock();
}

void NewController::setReadOnly(QList<int> IDs, bool readOnly)
{
    Key* readOnlyKey = constructKey("readOnly", QVariant::Bool, Entity::EK_ALL);


    QList<Node*> nodeList;
    //Construct a list of Nodes to be snippeted
    foreach(int ID, IDs){
        Node* node = getNodeFromID(ID);
        if(node){
            if(!nodeList.contains(node)){
                nodeList += node;
            }
            foreach(Node* child, node->getChildren()){
                if(!nodeList.contains(child)){
                    nodeList += child;
                }
            }
        }
    }
    //Attach read Only Data to the top.

    bool displayWarning = true;
    //Attach read only Data.
    foreach(Node* node, nodeList){
        if(node->isSnippetReadOnly() || node->getData("readOnlyDefinition")){
            if(displayWarning){
                displayWarning = false;
                emit controller_DisplayMessage(WARNING, "Entity in selection is a read-only snippet. Cannot modify read-only state.", "Cannot Modify Read-Only Snippet", "Snippet", node->getID());
            }
            continue;
        }
        Data* readOnlyData = node->getData(readOnlyKey);

        if(!readOnlyData){
            readOnlyData = new Data(readOnlyKey, readOnly);
            attachData(node, readOnlyData);
        }else{
            _setData(node, "readOnly", readOnly);
        }
    }



    emit controller_ActionFinished();
}

/**
 * @brief NewController::clear
 */
void NewController::clear()
{
    bool success = _clear();
    emit controller_ActionProgressChanged(100);
    emit controller_ActionFinished(success);
}

/**
 * @brief NewController::replicate Essentially copies and pastes the ID's in place.
 * @param IDs - The list of entity IDs
 */
void NewController::replicate(QList<int> IDs)
{
    lock.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canReplicate(selection)){
        success = _replicate(selection);
    }
    lock.unlock();
    emit controller_ActionFinished(success, "Cannot Replicate selection.");
}

/**
 * @brief NewController::cut Copies a selection of IDs and then deletes them.
 * @param selectedIDs - The list of entity IDs
 */
void NewController::cut(QList<int> IDs)
{
    lock.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canCut(selection)){
        QString data = _copy(selection);
        emit controller_SetClipboardBuffer(data);
        emit triggerAction("Cutting Selection");
        success = _remove(selection);
    }

    lock.unlock();
    emit controller_ActionFinished(success, "Cannot cut selection.");
}



/**
 * @brief NewController::paste Tells the Controller to Paste
 * @param ID - The ID of the node to paste into
 * @param xmlData - The GraphML Data to paste.
 */
void NewController::paste(QList<int> IDs, QString xmlData)
{
    lock.lockForWrite();
    QList<Entity*> selection = getOrderedSelection(IDs);

    bool success = false;
    if(canPaste(selection)){
        success = _paste(selection.first()->getID(), xmlData);
    }
    lock.unlock();
    emit controller_ActionFinished(success, "Cannot paste into selection");
}

/**
 * @brief NewController::_paste Pastes graphml Data into the Node specified by the ID provided.
 * @param ID - The ID of the node to paste into
 * @param xmlData - The GraphML Data to paste.
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool NewController::_paste(int ID, QString xmlData, bool addAction)
{
    bool success = true;

    Node* parentNode = getNodeFromID(ID);
    if(!parentNode){
        controller_DisplayMessage(WARNING, "Paste Error" ,"Please select an entity to paste into.", "Critical");
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
bool NewController::_cut(QList<int> IDs, bool addAction)
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
bool NewController::_copy(QList<int> IDs)
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
        emit controller_DisplayMessage(WARNING, "Cannot copy selection", "Copy Error", "Warning");
    }
    return success;
}

/**
 * @brief NewController::_remove - Removes the selection of GraphML Entities from their IDs
 * @param IDs - The ID's of the entities to remove.
 * @param addAction - Adds a Action in the Undo/Redo Stack
 * @return Action successful.
 */
bool NewController::_remove(QList<int> IDs, bool addAction)
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

bool NewController::_remove(QList<Entity *> items)
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

bool NewController::_remove(int ID, bool addAction)
{
    QList<int> IDs;
    IDs << ID;
    return _remove(IDs, addAction);
}

bool NewController::_replicate(QList<Entity *> items)
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
bool NewController::_replicate(QList<int> IDs, bool addAction)
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
bool NewController::_importProjects(QStringList xmlDataList, bool addAction)
{
    bool success = true;

    if(xmlDataList.length() > 0){
        controller_SetViewEnabled(false);
        if(addAction){
            triggerAction("Importing GraphML Projects.");
            emit controller_ActionProgressChanged(0, "Importing GraphML files");
        }

        foreach(QString xmlData, xmlDataList){
            //bool result = _importGraphMLXML(xmlData, getModel());
            bool result = _newImportGraphML(xmlData, getModel());
            if(!result){
                controller_DisplayMessage(CRITICAL, "Cannot import document", "Import Error", "Critical", getModel()->getID());
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
bool NewController::_importSnippet(QList<int> IDs, QString fileName, QString fileData, bool addAction)
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
QString NewController::_exportSnippet(QList<int> IDs)
{
    QString snippetData;
    if(canExportSnippet(IDs)){
        CUT_USED = false;

        QString parentNodeKind = "";

        Node* node = getFirstNodeFromList(IDs);
        if(node && node->getParentNode()){
            parentNodeKind = node->getParentNode()->getNodeKind();
        }

        bool readOnly = false;

        //Check if read only.
        if(parentNodeKind == "InterfaceDefinitions"){
            readOnly = askQuestion(MESSAGE, "Export as Read-Only Snippet?", "Would you like to export the current selection as a read-only snippet?");
        }

        //Construct the Keys to attach to the nodes to export.
        QList<Node*> nodeList;

        if(readOnly){
            //Get the information about this machine/time
            uint exportTimeStamp = getTimeStampEpoch();
            long long machineID = getMACAddress();

            //Construct the Keys for the data we need to attach.
            Key* readOnlyDefinitionKey = constructKey("readOnlyDefinition", QVariant::Bool, Entity::EK_NODE);
            Key* readOnlyKey = constructKey("readOnly", QVariant::Bool, Entity::EK_NODE);
            Key* IDKey = constructKey("snippetID", QVariant::Int, Entity::EK_NODE);

            Key* timeKey = constructKey("snippetTime", QVariant::LongLong, Entity::EK_NODE);
            Key* exportTimeKey = constructKey("exportTime", QVariant::LongLong, Entity::EK_NODE);
            Key* macKey = constructKey("snippetMAC", QVariant::LongLong, Entity::EK_NODE);


            //Construct a list of Nodes to be snippeted
            foreach(int ID, IDs){
                Node* node = getNodeFromID(ID);
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
            Key* readOnlyDefinitionKey = constructKey("readOnlyDefinition", QVariant::Bool, Entity::EK_NODE);
            Key* readOnlyKey = constructKey("readOnly", QVariant::Bool, Entity::EK_NODE);

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

long long NewController::getMACAddress()
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


/**
 * @brief NewController::getAdoptableNodeKinds Gets the list of NodeKinds that the node (From ID) can adopt.
 * @param ID The ID of the parent Node
 * @return A list of all adoptable node kinds.
 */
QStringList NewController::getAdoptableNodeKinds(int ID)
{
    QStringList adoptableNodeKinds;

    lock.lockForRead();
    Node* parent = getNodeFromID(ID);

    //Ignore all children for read only kind.
    if(parent && !parent->isReadOnly()){
        foreach(QString nodeKind, guiConstructableNodeKinds){
            Node* node = constructTypedNode(nodeKind, true);
            if(node){
                if(parent->canAdoptChild(node)){
                    adoptableNodeKinds.append(nodeKind);
                }
                //Clean up memory.
                delete node;
            }
        }
    }

    lock.unlock();
    return adoptableNodeKinds;
}


QList<int> NewController::getConnectableNodeIDs(QList<int> srcs, Edge::EDGE_CLASS edgeKind)
{

    QList<int> dstIDs;
    lock.lockForRead();
    foreach(Node* dst, _getConnectableNodes(getNodes(srcs), edgeKind)){
        dstIDs.append(dst->getID());
    }
    lock.unlock();
    return dstIDs;
}

QList<int> NewController::getConstructableConnectableNodes(int parentID, QString instanceNodeKind, Edge::EDGE_CLASS edgeClass)
{

    QList<int> dstIDs;

    lock.lockForRead();
    Node* parentNode = getNodeFromID(parentID);
    Node* childNode = constructTypedNode(instanceNodeKind, true);

    if(childNode && parentNode){
        if(parentNode->canAdoptChild(childNode)){
            parentNode->addChild(childNode);

            QList<Node*> source;
            source << childNode;

            foreach(Node* dst, _getConnectableNodes(source, edgeClass)){
                dstIDs.append(dst->getID());
            }
        }
    }
    if(childNode){
        delete childNode;
    }

    lock.unlock();
    return dstIDs;
}

QList<Node *> NewController::_getConnectableNodes(QList<Node *> sourceNodes, Edge::EDGE_CLASS edgeKind)
{
    QList<Node*> validNodes;




    foreach(Node* src, sourceNodes){
        if(!src->canAcceptEdgeClass(edgeKind)){
            sourceNodes.removeAll(src);
        }
    }

    if(!sourceNodes.isEmpty()){
        //Itterate through all nodes.
        foreach(Node* dst, getAllNodes()){
            //Ignore nodes which can't take this edge class.
            if(dst->canAcceptEdgeClass(edgeKind)){
                bool accepted = true;
                foreach(Node* src, sourceNodes){
                    if(src->canConnect(dst) != edgeKind){
                        accepted = false;
                        break;
                    }
                }
                if(accepted){
                    validNodes.append(dst);
                }
            }
        }
    }
    return validNodes;
}

QList<int> NewController::getOrderedSelectionIDs(QList<int> selection)
{
    QList<int> orderedSelection;

    foreach(Entity* item, getOrderedSelection(selection)){
        if(item){
            orderedSelection.append(item->getID());
        }
    }
    return orderedSelection;
}

QList<Entity*> NewController::getOrderedSelection(QList<int> selection)
{
    QList<Entity*> orderedSelection;

    foreach(int ID, selection){
        Entity* entity = getGraphMLFromID(ID);
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

QStringList NewController::getValidKeyValues(int nodeID, QString keyName)
{
    QStringList validKeyValues;
    lock.lockForRead();
    Key* key = getKeyFromName(keyName);
    if(key){
        QString nodeKind = "";
        if(nodeID != -1){
            Node* node = getNodeFromID(nodeID);
            nodeKind = node->getNodeKind();
        }

        validKeyValues = key->getValidValues(nodeKind);

    }
    lock.unlock();
    return validKeyValues;
}

QList<int> NewController::getInstances(int ID)
{
    QList<int> instances;
    Node* node = getNodeFromID(ID);
    if(node){
        foreach(Node* instance, node->getInstances()){
            if(instance){
                instances << instance->getID();
            }
        }
    }
    return instances;
}

int NewController::getAggregate(int ID)
{
    int aggrID = -1;
    Node* node = getNodeFromID(ID);
    if(node){
        EventPort* eventPort = dynamic_cast<EventPort*>(node);
        if(eventPort && eventPort->getAggregate()){
            aggrID = eventPort->getAggregate()->getID();
        }
    }
    return aggrID;

}

int NewController::getDeployedHardwareID(int ID)
{
    int deplID = -1;
    Node* node = getNodeFromID(ID);
    if(node){
        foreach(Edge* edge, node->getEdges(0)){
            if(edge->isDeploymentLink()){
                if(edge->getSource() == node){
                    deplID = edge->getDestination()->getID();
                }else{
                    deplID = edge->getSource()->getID();
                }
            }
        }
    }
    return deplID;
}

void NewController::setProjectSaved(QString path)
{
    setProjectDirty(false);
    if(path != ""){
        //Update the file save path.
        setProjectPath(path);
    }
}


QString NewController::getXMLAttribute(QXmlStreamReader &xml, QString attributeID)
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



Key *NewController::constructKey(QString name, QVariant::Type type, Entity::ENTITY_KIND entityKind)
{
    Key* newKey = new Key(name, type, Entity::EK_ALL);

    //Search for a matching Key. If we find one, remove the newly created Key
    foreach(Key* key, keys){
        if(key->equals(newKey)){
            delete newKey;
            return key;
        }
    }

    //Protect the Key if it meant to be protected
    if(protectedKeyNames.contains(name)){
        newKey->setProtected(true);
    }

    //Set the keys data so that we can distinguish between things for read only mode.
    if(visualKeyNames.contains(name)){
        newKey->setIsVisualData(true);
    }

    if(name == "type"){
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Attribute" << "Member" << "Variable";
        validValues << "Boolean" << "Byte" << "Char" << "WideChar" << "ShortInteger" << "LongInteger" << "LongLongInteger" << "UnsignedShortInteger" << "UnsignedLongInteger" << "UnsignedLongLongInteger" << "FloatNumber" << "DoubleNumber" << "LongDoubleNumber" << "GenericObject" << "GenericValue" << "GenericValueObject" << "String" << "WideString";
        newKey->addValidValues(validValues, keysValues);
        newKey->setAllowAllValues("Variable");

        keysValues.clear();;
        validValues.clear();
        keysValues << "PeriodicEvent";
        validValues << "Constant" << "Exponential";
        newKey->addValidValues(validValues, keysValues);
    }
    if(name == "middleware"){
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Model";
        validValues << "tao" << "rtidds" << "opensplice" << "coredx" << "tcpip" << "qpidpb" ;
        newKey->addValidValues(validValues, keysValues);
    }

    if (name == "actionOn") {
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Process";
        validValues << "Activate" << "Preprocess" << "Mainprocess" << "Postprocess" << "Passivate";
        newKey->addValidValues(validValues, keysValues);
    }
    if(name == "replicate_count"){
        QStringList keysValues;
        keysValues << "ComponentAssembly";
        QPair<qreal, qreal> range;
        range.first = 1;
        range.second = 999999;
        newKey->addValidRange(range, keysValues);
    }
    if(name == "folder" || name == "file"){
        QStringList invalidChars;
        invalidChars  << "|" << "," << "*" << "?" << "<" << ">" << ":";
        if(name == "file"){
            invalidChars << "/" << "\\" << "\t";
        }
        newKey->addInvalidCharacters(invalidChars);
    }
    if(name == "label"){
        QStringList invalidChars;
        invalidChars << "*" << "." << "[" << "]"<< ";" << "|" << "," <<  "%";
        invalidChars << "\"" << "'"  << "/" << "\\" << "=" << ":" << " " << "<" << ">" << "\t";
        newKey->addInvalidCharacters(invalidChars);
    }

    if(name.startsWith("qos_dds")){
        if(name == "qos_dds_kind"){
            {
               //HistoryQosPolicy
               QStringList kinds, values;
               kinds << "DDS_HistoryQosPolicy";
               values << "KEEP_LAST_HISTORY_QOS";
               values << "KEEP_ALL_HISTORY_QOS";
               newKey->addValidValues(values, kinds);
            }
            {
               //OwnershipQosPolicyKind
               QStringList kinds, values;
               kinds << "DDS_OwnershipQosPolicy";
               values << "SHARED_OWNERSHIP_QOS";
               values << "EXCLUSIVE_OWNERSHIP_QOS";
               newKey->addValidValues(values, kinds);
            }
            {
               //ReliabilityQosPolicyKind
               QStringList kinds, values;
               kinds << "DDS_ReliabilityQosPolicy";
               values << "BEST_EFFORT_RELIABILITY_QOS";
               values << "RELIABLE_RELIABILITY_QOS";
               newKey->addValidValues(values, kinds);
            }
            {
               //LivelinessQosPolicyKind
               QStringList kinds, values;
               kinds << "DDS_LivelinessQosPolicy";
               values << "AUTOMATIC_LIVELINESS_QOS";
               values << "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS";
               values << "MANUAL_BY_TOPIC_LIVELINESS_QOS";
               newKey->addValidValues(values, kinds);
            }
            {
               //DurabilityQosPolicyKind
               QStringList kinds, values;
               kinds << "DDS_DurabilityQosPolicy";
               values << "VOLATILE_DURABILITY_QOS";
               values << "TRANSIENT_LOCAL_DURABILITY_QOS";
               values << "TRANSIENT_DURABILITY_QOS";
               values << "PERSISTENT_DURABILITY_QOS";
               newKey->addValidValues(values, kinds);
            }
            {
               //DestinationOrderQosPolicyKind
               QStringList kinds, values;
               kinds << "DDS_DestinationOrderQosPolicy";
               values << "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS";
               values << "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS";
               newKey->addValidValues(values, kinds);
            }
        }else if(name == "qos_dds_access_scope"){
            {
               //PresentationQosPolicyAccessScopeKind
               QStringList kinds, values;
               kinds << "DDS_PresentationQosPolicy";
               values << "INSTANCE_PRESENTATION_QOS";
               values << "TOPIC_PRESENTATION_QOS";
               values << "GROUP_PRESENTATION_QOS";
               newKey->addValidValues(values, kinds);
            }
        }else if(name == "qos_dds_access_scope"){
            {
               //PresentationQosPolicyAccessScopeKind
               QStringList kinds, values;
               kinds << "DDS_PresentationQosPolicy";
               values << "INSTANCE_PRESENTATION_QOS";
               values << "TOPIC_PRESENTATION_QOS";
               values << "GROUP_PRESENTATION_QOS";
               newKey->addValidValues(values, kinds);
            }
        }else if(name == "qos_dds_history_kind"){
            {
               //HistoryQosPolicy
               QStringList kinds, values;
               kinds << "DDS_DurabilityServiceQosPolicy";
               values << "KEEP_LAST_HISTORY_QOS";
               values << "KEEP_ALL_HISTORY_QOS";
               newKey->addValidValues(values, kinds);
            }
        }
    }


    connect(newKey, SIGNAL(validateError(QString,QString,int)), this, SLOT(displayMessage(QString,QString,int)));
    //Add it to the list of Keys.
    if(keys.contains(name)){
        qCritical() << "Got Duplicate Keys: " << name;
    }else{
        keys[name] = newKey;
    }

    newKey->setParent(this);
    //Construct an Action to reverse the update
    EventAction action = getEventAction();
    action.ID = newKey->getID();
    action.Action.type = CONSTRUCTED;
    action.Action.kind = newKey->getGraphMLKind();
    action.Key.kind = entityKind;
    action.Key.type = type;

    addActionToStack(action);

    return newKey;
}

bool NewController::destructKey(QString name)
{
    Key* key = getKeyFromName(name);
    if(key){

        //Construct an Action to reverse the update
        EventAction action = getEventAction();
        action.ID = key->getID();
        action.Action.type = DESTRUCTED;
        action.Action.kind = key->getGraphMLKind();
        action.Key.kind = key->getEntityKind();
        action.Key.type = key->getType();

        addActionToStack(action);

        keys.remove(name);
        delete key;
        return true;
    }
    return false;
}

Key *NewController::getKeyFromName(QString name)
{
    if(keys.contains(name)){
        return keys[name];
    }
    return 0;
}

Key *NewController::getKeyFromID(int ID)
{
    foreach(Key* key, keys){
        if(key->getID() == ID){
            return key;
        }
    }
    return 0;
}


Edge *NewController::_constructEdge(Node *src, Node *dst)
{
    if(src && dst){
        Edge::EDGE_CLASS edgeClass = src->canConnect(dst);

        if(edgeClass != Edge::EC_NONE){
            Edge* edge = constructTypedEdge(src, dst, edgeClass);

            if(edge){
                //Attach required data.
                _attachData(edge, constructRequiredEdgeData(edgeClass), false);
            }

            return edge;
        }else{
            if(!src->gotEdgeTo(dst)){
                qCritical() << "Edge: Source: " << src->toString() << " to Destination: " << dst->toString() << " Cannot be created!";
            }
        }
    }
    return 0;
}

void NewController::storeGraphMLInHash(Entity* item)
{
    if(!item){
        return;
    }

    int ID = item->getID();

    if(IDLookupGraphMLHash.contains(ID)){
        qCritical() << "Hash already contains item with ID: " << ID;
    }else{
        //Add it to hash.
        IDLookupGraphMLHash[ID] = item;

        QString kind = item->getDataValue("kind").toString();

        Entity::ENTITY_KIND entityKind = item->getEntityKind();
        Node* node = 0;
        Edge* edge = 0;
        if(entityKind == Entity::EK_NODE){
            node = (Node*)item;

            kindLookup[kind].append(ID);
            reverseKindLookup[ID] = kind;

            QString treeIndexStr = ((Node*)item)->getTreeIndexString();

            treeHash.insert(treeIndexStr, ID);

            nodeIDs.append(ID);
        }else if(entityKind == Entity::EK_EDGE){
            edge = (Edge*)item;
            edgeIDs.append(ID);
        }


        //Connect things!
        connect(item, &Entity::dataChanged, this, &NewController::dataChanged);
        connect(item, &Entity::dataRemoved, this, &NewController::dataRemoved);
        connect(item, &Entity::propertyChanged, this, &NewController::propertyChanged);
        connect(item, &Entity::propertyRemoved, this, &NewController::propertyRemoved);

        QHash<QString, QVariant> data;
        QHash<QString, QVariant> properties;

        foreach(QString key, item->getKeyNames()){
            data[key] = item->getDataValue(key);
        }

        if(entityKind == Entity::EK_NODE){
            properties["viewAspect"] = node->getViewAspect();
            properties["treeIndex"] = node->getTreeIndexAlpha();
            properties["parentID"] = node->getParentNodeID();
        }else if(entityKind == Entity::EK_EDGE){
            properties["srcID"] = edge->getSourceID();
            properties["dstID"] = edge->getDestinationID();
            properties["edgeClass"] = edge->getEdgeClass();
        }

        properties["protectedKeys"] = item->getProtectedKeys();

        ENTITY_KIND ek = EK_NODE;

        if(item->isEdge()){
            ek = EK_EDGE;
        }

        emit entityConstructed(ID, ek, kind, data, properties);
    }
}

Entity*NewController::getGraphMLFromHash(int ID)
{
    if(IDLookupGraphMLHash.contains(ID)){
        return IDLookupGraphMLHash[ID];
    }else{
        //qCritical() << "Cannot find GraphML from Lookup Hash. ID: " << ID;
    }
    return 0;
}

void NewController::removeGraphMLFromHash(int ID)
{
    if(IDLookupGraphMLHash.contains(ID)){
        Entity* item = IDLookupGraphMLHash[ID];
        if(item){
            Entity::ENTITY_KIND entityKind = item->getEntityKind();
            QString kind = item->getDataValue("kind").toString();



            Node* node = 0;
            Edge* edge = 0;

            ENTITY_KIND ek = EK_NODE;
            if(item->isNode()){
                node = (Node*) item;
            }else{
                edge = (Edge*) item;
                ek = EK_EDGE;
            }
            emit entityDestructed(ID, ek, kind);

            if(node){
                QString nodeLabel = node->getDataValue("label").toString();


                if(kind.startsWith("Hardware")){
                    hardwareEntities.remove(nodeLabel);
                }else if(kind == "ManagementComponent"){
                    managementComponents.remove(nodeLabel);
                }else if(kind == "Process"){
                    if(_isInWorkerDefinitions(node)){
                        //If we are removing a Process contained in the WorkerDefinitions section.
                        QString processName = getProcessName((Process*)node);
                        if(processName != ""){
                            workerProcesses.remove(processName);
                        }
                    }
                }
            }
        }
    }


    if(reverseKindLookup.contains(ID)){
        QString kind = reverseKindLookup[ID];
        if(kind != ""){
            kindLookup[kind].removeAll(ID);
            reverseKindLookup.remove(ID);
        }
    }


    if(treeHash.containsValue(ID)){
        treeHash.removeValue(ID);
    }

    if(readOnlyHash.containsValue(ID)){
        readOnlyHash.removeValue(ID);
    }

    if(reverseReadOnlyLookup.contains(ID)){
        int originalID = reverseReadOnlyLookup[ID];
        reverseReadOnlyLookup.remove(ID);
        readOnlyLookup.remove(originalID);
    }

    IDLookupGraphMLHash.remove(ID);

    nodeIDs.removeOne(ID);
    edgeIDs.removeOne(ID);



    if(IDLookupGraphMLHash.size() != (nodeIDs.size() + edgeIDs.size())){
        qCritical() << "Hash Map Inconsistency detected!";
    }
}

Node *NewController::constructChildNode(Node *parentNode, QList<Data *> nodeData)
{
    //Construct the node with the data.
    Node* node = _constructNode(nodeData);

    if(!node){
        qCritical() << "Node was not successfully constructed!";
        return 0;
    }



    //If we have no parentNode, attempt to attach it to the Model.
    if(!parentNode){
        parentNode = model;
    }

    bool attached = attachChildNode(parentNode, node);

    if(!attached){
        destructNode(node);
        node = 0;
    }
    return node;
}

bool NewController::attachChildNode(Node *parentNode, Node *node, bool sendGUIRequest)
{
    bool inModel = _isInModel(node);

    if(!inModel){
        if(parentNode->canAdoptChild(node)){
            //Attach new node to parent.
            parentNode->addChild(node);

            //Only enforce unique-ness for non-read-only nodes.
            if(!node->isReadOnly() && node->hasData("label")){
                //Force Unique labels
                enforceUniqueLabel(node);
            }

            //Force Unique sort order
            enforceUniqueSortOrder(node);

            if(sendGUIRequest){
                constructNodeGUI(node);
            }
        }else{
            return false;

        }
    }else{
        return node->getParentNode() == parentNode;
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
            constructDependantRelative(dependant, node);
        }
    }

    return true;
}

Node *NewController::_constructNode(QList<Data *> nodeData)
{
    //Get the Kind from the data.
    QString childNodeKind = getDataValueFromKeyName(nodeData, "kind");
    QString childNodeType = getDataValueFromKeyName(nodeData, "type");

    QString childUniqueID = getDataValueFromKeyName(nodeData, "label");;
    if(childNodeKind.startsWith("Hardware")){
        //Use the URL, but fall back on the label if nothing is in url
        QString url = getDataValueFromKeyName(nodeData, "url");
        if(url != ""){
            childUniqueID = url;
        }
    }

    Node* node = constructTypedNode(childNodeKind, false, childNodeType, childUniqueID);

    //Enforce Default Data!
    QList<Data*> requiredData = constructDataVector(childNodeKind);


    bool inModel = _isInModel(node);
    if(node){
        //Attach Default Data.
        _attachData(node, requiredData, inModel);

        //Update Data with custom Data!
        _attachData(node, nodeData, inModel);


    }

    //Delete the Data objects which didn't get adopted to the Node (or if our Node is null)
    while(!requiredData.isEmpty()){
        Data* data = requiredData.takeFirst();

        if(!node || data->getParent() != node){
            delete data;
        }
    }

    //Delete the Data objects which didn't get adopted to the Node (or if our Node is null)
    while(!nodeData.isEmpty()){
        Data* data = nodeData.takeFirst();

        if(!node || data->getParent() != node){
            delete data;
        }
    }
    return node;
}

bool NewController::updateProgressNotification()
{
    if(OPENING_PROJECT || IMPORTING_PROJECT){
           return true;
   }
    return false;
}

QList<int> NewController::getIDs(QList<Entity *> items)
{
    QList<int> IDs;
    foreach(Entity* item, items){
        IDs.append(item->getID());
    }
    return IDs;
}


QList<Data *> NewController::cloneNodesData(Node *original, bool ignoreVisuals)
{
    QStringList ignoredKeys;
    if(ignoreVisuals){
        ignoredKeys << "x" << "y" << "isExpanded" << "readOnly" << "width" << "height" << "sortOrder";
    }

    QList<Data*> clonedData;

    //Clone the data from the Definition.
    foreach(Data* data, original->getData()){
        if(!ignoredKeys.contains(data->getKeyName())){
            //Clone the data.
            clonedData << Data::clone(data);
        }
    }
    return clonedData;
}

Node *NewController::cloneNode(Node *original, Node *parent, bool ignoreVisuals)
{
    QList<Data*> clonedData = cloneNodesData(original, ignoreVisuals);
    Node* newNode = constructChildNode(parent, clonedData);

    if(newNode){
        foreach(Node* child, original->getChildren(0)){
            cloneNode(child, newNode);
        }
    }
    return newNode;
}


QList<Data *> NewController::constructDataVector(QString nodeKind, QPointF relativePosition, QString nodeType, QString nodeLabel)
{
    Key* kindKey = constructKey("kind", QVariant::String, Entity::EK_NODE);
    Key* labelKey = constructKey("label", QVariant::String, Entity::EK_NODE);
    Key* typeKey = constructKey("type", QVariant::String, Entity::EK_NODE);
    Key* widthKey = constructKey("width", QVariant::Double, Entity::EK_NODE);
    Key* heightKey = constructKey("height", QVariant::Double, Entity::EK_NODE);
    Key* sortKey = constructKey("sortOrder", QVariant::Int, Entity::EK_NODE);
    Key* expandedKey = constructKey("isExpanded", QVariant::Bool, Entity::EK_NODE);

    QList<Data*> data;

    data.append(constructPositionDataVector(relativePosition));


    QString labelString = nodeKind;

    if(nodeLabel != ""){
        labelString = nodeLabel;
    }
    if(nodeKind == "Model"){
        labelString = "Untitled";
    }

    if(nodeKind.endsWith("Parameter")){
        labelString = "";
    }

    data.append(new Data(kindKey, nodeKind));

    if(nodeKind.contains("Edge")){
        data.append(new Data(widthKey, -1));
        data.append(new Data(heightKey, -1));
    }

    QStringList protectedLabels;
    protectedLabels << "Parameter" << "ManagementComponent";
    protectedLabels.append(dds_QosNodeKinds);

    bool isDDSQOS = dds_QosNodeKinds.contains(nodeKind);
    bool protectLabel = protectedLabels.contains(nodeKind);
    bool customLabel = isDDSQOS;

    Data* labelData = new Data(labelKey);
    labelData->setValue(labelString);
    labelData->setProtected(protectLabel);



    data.append(new Data(sortKey, -1));


    //Attach Node Specific Data.


    if(nodeKind != "Model" && !nodeKind.endsWith("Definitions")){
        data.append(new Data(expandedKey, false));
    }

    if(nodeKind == "ManagementComponent"){
        Data* typeData = new Data(typeKey, nodeType);
        typeData->setProtected(true);
        data.append(typeData);
        if(nodeType == DDS_LOGGING_SERVER){
            Key* frequencyKey = constructKey("frequency", QVariant::Double, Entity::EK_NODE);
            Key* localLoggingKey = constructKey("cached_logging", QVariant::Bool, Entity::EK_NODE);
            Key* processLoggingKey = constructKey("processes_to_log", QVariant::String, Entity::EK_NODE);
            Key* batchSizeKey = constructKey("sql_batch_size", QVariant::Int, Entity::EK_NODE);

            Key* topicKey = constructKey("topic_name", QVariant::String, Entity::EK_NODE);
            Key* domainKey = constructKey("domain", QVariant::Int, Entity::EK_NODE);

            //Set Defaults
            Data* freqData = new Data(frequencyKey, 1);
            Data* localData = new Data(localLoggingKey, false);
            Data* topicData = new Data(topicKey, "DIGSystemMonitor");
            Data* domainData = new Data(domainKey, 9);
            Data* batchData = new Data(batchSizeKey, 1000);
            Data* processData = new Data(processLoggingKey, "dance_node_manager,dance_execution_manager,dig-sls,dig-slc");

            data.append(freqData);
            data.append(localData);
            data.append(topicData);
            data.append(domainData);
            data.append(processData);
            data.append(batchData);
        }
    }

    QStringList editableTypeKinds;
    editableTypeKinds << "Variable" << "Member" << "Attribute";

    if(editableTypeKinds.contains(nodeKind)){
        Data* typeData = new Data(typeKey, "String");
        typeData->setProtected(false);
        data.append(typeData);
    }

    if(nodeKind == "Model"){
        Key* middlewareKey = constructKey("middleware", QVariant::String, Entity::EK_NODE);
        Key* projectUUID = constructKey("projectUUID", QVariant::String, Entity::EK_NODE);
        Data* projectData = new Data(projectUUID);
        projectData->setValue(getTimeStamp());
        Data* middlewareData = new Data(middlewareKey);
        middlewareData->setValue("tao");
        data.append(projectData);
        data.append(middlewareData);
    }
    if(nodeKind == "PeriodicEvent"){
        Key* frequencyKey = constructKey("frequency", QVariant::Double, Entity::EK_NODE);
        Data* freqData = new Data(frequencyKey);

        Data* typeData = new Data(typeKey, "Constant");
        typeData->setProtected(false);
        data.append(typeData);

        freqData->setValue(1.0);
        data.append(freqData);
    }
    if(nodeKind == "Process"){
        Key* actionOnKey = constructKey("actionOn", QVariant::String,Entity::EK_NODE);

        if(isUserAction() && !CONSTRUCTING_WORKERFUNCTION){
            //If this is a user action, this is a blank process
            Key* fileKey = constructKey("file", QVariant::String,Entity::EK_NODE);
            Key* folderKey = constructKey("folder", QVariant::String,Entity::EK_NODE);
            Key* operationKey = constructKey("operation", QVariant::String,Entity::EK_NODE);

            Key* parametersKey = constructKey("parameters", QVariant::String,Entity::EK_NODE);
            Key* codeKey = constructKey("code", QVariant::String,Entity::EK_NODE);

            Key* workerKey = constructKey("worker", QVariant::String,Entity::EK_NODE);
            Key* workerIDKey = constructKey("workerID", QVariant::String,Entity::EK_NODE);

            data.append(new Data(fileKey, "File"));
            data.append(new Data(folderKey, "Folder"));
            data.append(new Data(operationKey, "Operation"));
            data.append(new Data(workerKey, "Worker"));
            data.append(new Data(parametersKey, "/*USED FOR BLANK FUNCTIONS*/"));
            data.append(new Data(codeKey, ""));
            data.append(new Data(workerIDKey, "workerID"));
        }

        data.append(new Data(actionOnKey, "Mainprocess"));
    }
    if(nodeKind == "Condition"){
        Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
        data.append(new Data(valueKey));
    }
    if(nodeKind == "Member"){
        Key* keyKey = constructKey("key", QVariant::Bool,Entity::EK_NODE);
        Data* keyData = new Data(keyKey);
        keyData->setValue(false);
        data.append(keyData);
    }
    if(nodeKind == "MemberInstance"){
        Key* keyKey = constructKey("key", QVariant::Bool,Entity::EK_NODE);
        Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
        Data* keyData = new Data(keyKey);
        keyData->setValue(false);
        data.append(keyData);
        data.append(new Data(valueKey));
    }
    if(nodeKind == "HardwareNode"){
        Key* osKey = constructKey("os", QVariant::String,Entity::EK_NODE);
        Key* osVKey = constructKey("os_version", QVariant::String,Entity::EK_NODE);
        Key* ipKey = constructKey("ip_address", QVariant::String,Entity::EK_NODE);
        Key* archKey = constructKey("architecture", QVariant::String,Entity::EK_NODE);
        Key* descriptionKey = constructKey("description", QVariant::String,Entity::EK_NODE);
        data.append(new Data(osKey));
        data.append(new Data(osVKey));
        data.append(new Data(ipKey));
        data.append(new Data(archKey));
        data.append(new Data(descriptionKey));
    }
    if(nodeKind == "Attribute"){
        data.append(new Data(typeKey, "String"));
    }
    if(nodeKind == "ComponentAssembly"){
        Key* replicationKey = constructKey("replicate_count", QVariant::Int,Entity::EK_NODE);
        data.append(new Data(replicationKey, "1"));
    }

    if(nodeKind == "Vector"){
        Key* sizeKey = constructKey("max_size", QVariant::Int,Entity::EK_NODE);
        data.append(new Data(sizeKey, "0"));
    }

    if(nodeKind == "AttributeInstance"){
        Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
        data.append(new Data(valueKey));
    }



    if(nodeKind == "Variable"){
        Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
        data.append(new Data(valueKey));
    }
    if(nodeKind == "OutEventPortInstance" || nodeKind == "InEventPortInstance"){
        Key* topicKey = constructKey("topicName",QVariant::String,Entity::EK_NODE);
        data.append(new Data(topicKey));
    }

    if(nodeKind.endsWith("Instance") ||nodeKind.endsWith("Impl")){
        data.append(new Data(typeKey));
    }

    if(nodeKind.contains("EventPort")){
        Data* typeData = new Data(typeKey);
        if(nodeKind.endsWith("EventPort")){
            typeData->setProtected(true);
        }
        data.append(typeData);
    }
    if(nodeKind == "InEventPort"){
        Key* asyncKey = constructKey("async", QVariant::Bool,Entity::EK_NODE);
        data.append(new Data(asyncKey, true));
    }

    if(nodeKind.endsWith("Parameter")){
        data.append(new Data(typeKey));

        if(nodeKind == "InputParameter"){
            Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
            data.append(new Data(valueKey));
        }

    }

    if(dds_QosNodeKinds.contains(nodeKind)){
        Key* duration_key = constructKey("qos_dds_duration", QVariant::Double, Entity::EK_NODE);
        Key* kind_key = constructKey("qos_dds_kind", QVariant::String, Entity::EK_NODE);
        Key* int_value_key = constructKey("qos_dds_int_value", QVariant::Int, Entity::EK_NODE);
        Key* str_value_key = constructKey("qos_dds_str_value", QVariant::String, Entity::EK_NODE);
        Key* max_samples_key = constructKey("qos_dds_max_samples", QVariant::Int, Entity::EK_NODE);
        Key* max_instances_key = constructKey("qos_dds_max_instances", QVariant::Int, Entity::EK_NODE);
        Key* max_samples_per_key = constructKey("qos_dds_max_samples_per_instance", QVariant::Int, Entity::EK_NODE);


        QString hrLabel = nodeKind;
        hrLabel = hrLabel.replace("QosPolicy", "");;
        hrLabel = hrLabel.replace("DDS_", "");
        hrLabel = hrLabel.replace("_", " ");

        labelData->setValue(hrLabel);


        if(nodeKind == "DDS_DeadlineQosPolicy"){
            Key* period_key = constructKey("qos_dds_period", QVariant::String, Entity::EK_NODE);
            data.append(new Data(period_key, 1));
        }else if(nodeKind == "DDS_DestinationOrderQosPolicy"){
            data.append(new Data(kind_key, "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"));
        }else if(nodeKind == "DDS_DurabilityQosPolicy"){
            data.append(new Data(kind_key, "VOLATILE_DURABILITY_QOS"));
        }else if(nodeKind == "DDS_DurabilityServiceQosPolicy"){
            Key* service_key = constructKey("qos_dds_service_cleanup_delay", QVariant::Double, Entity::EK_NODE);
            Key* history_kind_key = constructKey("qos_dds_history_kind", QVariant::String, Entity::EK_NODE);
            Key* history_depth_key = constructKey("qos_dds_history_depth", QVariant::Int, Entity::EK_NODE);

            data.append(new Data(service_key, 1.0));
            data.append(new Data(history_kind_key, "KEEP_LAST_HISTORY_QOS"));
            data.append(new Data(history_depth_key, 1));
            data.append(new Data(max_samples_key, 1));
            data.append(new Data(max_instances_key, 1));
            data.append(new Data(max_samples_per_key, 1));
        }else if(nodeKind == "DDS_DurabilityQosPolicy"){
            data.append(new Data(kind_key, "VOLATILE_DURABILITY_QOS"));
        }else if(nodeKind == "DDS_EntityFactoryQosPolicy"){
            Key* autoenable_key = constructKey("qos_dds_autoenable_created_entities", QVariant::Bool, Entity::EK_NODE);
            data.append(new Data(autoenable_key, true));
        }else if(nodeKind == "DDS_GroupDataQosPolicy"){
            data.append(new Data(str_value_key));
        }else if(nodeKind == "DDS_HistoryQosPolicy"){
            Key* depth_key = constructKey("qos_dds_depth", QVariant::Int, Entity::EK_NODE);
            data.append(new Data(kind_key, "KEEP_LAST_HISTORY_QOS"));
            data.append(new Data(depth_key, 1));
        }else if(nodeKind == "DDS_LatencyBudgetQosPolicy"){
            data.append(new Data(duration_key, 1.0));
        }else if(nodeKind == "DDS_LifespanQosPolicy"){
            data.append(new Data(duration_key, 1.0));
        }else if(nodeKind == "DDS_LivelinessQosPolicy"){
            Key* lease_duration_key = constructKey("qos_dds_lease_duration", QVariant::Double, Entity::EK_NODE);
            data.append(new Data(kind_key, "AUTOMATIC_LIVELINESS_QOS"));
            data.append(new Data(lease_duration_key, 1.0));
        }else if(nodeKind == "DDS_OwnershipQosPolicy"){
            data.append(new Data(kind_key, "SHARED_OWNERSHIP_QOS"));
        }else if(nodeKind == "DDS_OwnershipStrengthQosPolicy"){
            data.append(new Data(int_value_key, 0));
        }else if(nodeKind == "DDS_PartitionQosPolicy"){
            Key* name_key = constructKey("qos_dds_name", QVariant::String, Entity::EK_NODE);
            data.append(new Data(name_key));
        }else if(nodeKind == "DDS_PresentationQosPolicy"){
            Key* access_scope_key = constructKey("qos_dds_access_scope", QVariant::String, Entity::EK_NODE);
            Key* coherant_access_key = constructKey("qos_dds_coherent_access", QVariant::Bool, Entity::EK_NODE);
            Key* ordered_access_key = constructKey("qos_dds_ordered_access", QVariant::Bool, Entity::EK_NODE);
            data.append(new Data(access_scope_key, "INSTANCE_PRESENTATION_QOS"));
            data.append(new Data(coherant_access_key, false));
            data.append(new Data(ordered_access_key, false));
        }else if(nodeKind == "DDS_ReaderDataLifecycleQosPolicy"){
            Key* autopurge_nowriter_key = constructKey("qos_dds_autopurge_nowriter_samples_delay", QVariant::Double, Entity::EK_NODE);
            Key* autopurge_disposed_key = constructKey("qos_dds_autopurge_disposed_samples_delay", QVariant::Double, Entity::EK_NODE);
            data.append(new Data(autopurge_nowriter_key, 1.0));
            data.append(new Data(autopurge_disposed_key, 1.0));
        }else if(nodeKind == "DDS_ReliabilityQosPolicy"){
            Key* max_blocking_key = constructKey("qos_dds_max_blocking_time", QVariant::Double, Entity::EK_NODE);
            data.append(new Data(kind_key, "BEST_EFFORT_RELIABILITY_QOS"));
            data.append(new Data(max_blocking_key, 1.0));
        }else if(nodeKind == "DDS_ResourceLimitsQosPolicy"){
            data.append(new Data(max_samples_key, 1));
            data.append(new Data(max_instances_key, 1));
            data.append(new Data(max_samples_per_key, 1));
        }else if(nodeKind == "DDS_TimeBasedFilterQosPolicy"){
            Key* minimum_separation_key = constructKey("qos_dds_minimum_separation", QVariant::Double, Entity::EK_NODE);
            data.append(new Data(minimum_separation_key, -1));
        }else if(nodeKind == "DDS_TopicDataQosPolicy"){
            data.append(new Data(str_value_key));
        }else if(nodeKind == "DDS_TransportPriorityQosPolicy"){
            data.append(new Data(int_value_key, 0));
        }else if(nodeKind == "DDS_UserDataQosPolicy"){
            data.append(new Data(str_value_key));
        }else if(nodeKind == "DDS_WriterDataLifecycleQosPolicy"){
            Key* autodispose_key = constructKey("qos_dds_autodispose_unregistered_instances", QVariant::Bool, Entity::EK_NODE);
            data.append(new Data(autodispose_key, true));
        }
    }

    data.append(labelData);

    return data;
}

QList<Data *> NewController::constructRequiredEdgeData(Edge::EDGE_CLASS edgeClass)
{
    QList<Data*> dataList;

    Key* kindKey = constructKey("kind", QVariant::String, Entity::EK_ALL);
    QString kind = Edge::getKind(edgeClass);

    dataList.append(new Data(kindKey, kind));

    QString label;

    switch(edgeClass){
    case Edge::EC_AGGREGATE:{
        label = "Uses aggregate";
        break;
    }
    case Edge::EC_ASSEMBLY:{
        label = "Connected To";
        break;
    }
    case Edge::EC_DATA:{
        label = "Data linked";
        break;
    }
    case Edge::EC_DEFINITION:{
        label = "Is dependant on";
        break;
    }
    case Edge::EC_DEPLOYMENT:{
        label = "Is deployed on";
        break;
    }
    case Edge::EC_QOS:{
        label = "Uses QOS profile";
        break;
    }
    default:
        break;
    }

    if(!label.isEmpty()){
        Key* labelKey = constructKey("label", QVariant::String, Entity::EK_ALL);
        dataList.append(new Data(labelKey, label));
    }

    return dataList;
}

QList<Data *> NewController::constructPositionDataVector(QPointF point)
{
    Key* xKey = constructKey("x", QVariant::Double, Entity::EK_ALL);
    Key* yKey = constructKey("y", QVariant::Double, Entity::EK_ALL);

    QList<Data*> position;
    Data* xData = new Data(xKey);
    Data* yData = new Data(yKey);
    xData->setValue(point.x());
    yData->setValue(point.y());
    position << xData << yData;
    return position;
}

QString NewController::getNodeInstanceKind(Node *definition)
{
    QString definitionKind = definition->getDataValue("kind").toString();
    QString kindModifier = "Instance";

    QString instanceKind = definitionKind + kindModifier;

    //Instance of a AggregateInstance/MemberInstance is a AggregateInstance/MemberInstance
    if(definitionKind.endsWith("Instance")){
        instanceKind = definitionKind;
    }

    return instanceKind;
}

QString NewController::getNodeImplKind(Node *definition)
{
    QString definitionKind = definition->getDataValue("kind").toString();
    QString kindModifier = "Impl";

    QString implKind = definitionKind + kindModifier;

    if(definitionKind.endsWith("Instance")){
        implKind = definitionKind;
    }

    return implKind;
}



int NewController::constructDependantRelative(Node *parent, Node *definition)
{
    bool isInstance = parent->isInstance();
    int nodesMatched = 0;

    QString dependantKind = "";

    if(isInstance){
        dependantKind = getNodeInstanceKind(definition);
    }else{
        dependantKind = getNodeImplKind(definition);
    }



    //For each child in parent, check to see if any Nodes match Label/Type
    foreach(Node* child, parent->getChildrenOfKind(dependantKind, 0)){
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
            Edge* connectingEdge = constructEdgeWithData(child, definition);

            if(!connectingEdge){
                qCritical() << "constructDefinitionRelative(): Couldn't construct Edge between Relative Node and Definition Node.";
                continue;
            }

            nodesMatched++;
        }
    }

    //If we didn't find a match, we must create an Instance.
    if(nodesMatched == 0){
        Node *instanceNode = constructChildNode(parent, constructDataVector(dependantKind));

        if(!instanceNode){
            return 0;
        }

        Edge* connectingEdge = constructEdgeWithData(instanceNode, definition);

        if(!connectingEdge){
            return 0;
        }

        nodesMatched ++;
    }

    return nodesMatched;
}




void NewController::enforceUniqueLabel(Node *node, QString newLabel)
{
    if(!node){
        return;
    }
    if(newLabel == ""){
        newLabel = node->getDataValue("label").toString();
    }

    //Get root String
    if(requiresUniqueLabel(node)){
        bool gotMatches = false;
        bool gotExactMatch = false;
        QList<int> duplicateNumbers;
        int duplicateCount = 0;

        QRegularExpression regex("^"+ newLabel+"(_?)([0-9]+)?$");

        //If we have no parent node we don't need to enforce unique labels.
        foreach(Node* sibling, node->getSiblings()){
            QString siblingLabel = sibling->getDataValue("label").toString();

            QRegularExpressionMatch match = regex.match(siblingLabel);
            if(match.hasMatch()){
                QString underscore = match.captured(1);
                QString numberStr = match.captured(2);

                duplicateCount ++;
                if(siblingLabel == newLabel){
                    gotExactMatch = true;
                    gotMatches = true;
                    continue;
                }
                if(!underscore.isEmpty() && !numberStr.isEmpty()){
                    bool isInt = false;
                    int number = numberStr.toInt(&isInt);
                    if(isInt){
                        gotMatches = true;
                        duplicateNumbers += number;
                    }
                }
            }
        }

        qSort(duplicateNumbers);


        if(gotMatches){
            if(gotExactMatch){
                int newNumber = duplicateCount;
                for(int i = 0; i < duplicateCount; i ++){
                    if(!duplicateNumbers.contains(i)){
                        newNumber = i;
                        break;
                    }
                }
                newLabel = newLabel % "_" % QString::number(newNumber);
            }
        }
    }
    node->setDataValue("label", newLabel);
}

bool NewController::requiresUniqueLabel(Node *node)
{
    if(node->getNodeKind() == "Process"){
        return false;
    }
    return true;
}

/**
 * @brief NewController::enforceUniqueSortOrder
 * @param node
 * @param newPosition
 */
void NewController::enforceUniqueSortOrder(Node *node, int newPosition)
{
    if(!node){
        return;
    }
    Node* parentNode = node->getParentNode();
    if(!parentNode){
        return;
    }

    //If we have been given a position of -1 use the current value of sortOrder.
    if(newPosition == -1){
        newPosition = node->getDataValue("sortOrder").toInt();
    }

    int maxPos = parentNode->childrenCount() - 1;

    //If the newPosition is -1 or is bigger than our maxPos, put it last.
    if(newPosition > maxPos || newPosition == -1){
        newPosition = maxPos;
    }

    //Don't set Below 0
    if(newPosition < 0){
        newPosition = 0;
    }

    //Set the position
    node->setDataValue("sortOrder", newPosition);

    QList<Node*> siblings = parentNode->getChildren(0);
    siblings.removeAll(node);

    int siblingIndex = 0;
    for(int sortOrder = 0; sortOrder < siblings.count() + 1; sortOrder ++){
        if(sortOrder != newPosition){
            siblings[siblingIndex]->setDataValue("sortOrder", sortOrder);
            siblingIndex ++;
        }
    }
}

/**
 * @brief NewController::destructNode Fully deletes a Node; Including all of it's dependants and Edges etc.
 * @param node The Node to Delete.
 * @return Success
 */
bool NewController::destructNode(Node *node)
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





    //Get a list of dependants.
    QList<Node*> dependants = node->getDependants();
    //Remove all nodes which depend on this.
    while(!dependants.isEmpty()){
        Node* dependant = dependants.takeFirst();
        qCritical() << "Tearing down Dependants: " << dependant;
        destructNode(dependant);
    }

    QList<Edge*> edges = node->getEdges();
    //Remove all Edges.
    while(!edges.isEmpty()){
        Edge* edge = edges.takeLast();
        qCritical() << "Destructing Edge: " << edge;
        destructEdge(edge);
    }

    //Remove all Children.
    while(node->hasChildren()){
        Node* child = node->getFirstChild();
        qCritical() << "Tearing down Children: " << child;
        destructNode(child);
    }

    if(addAction){
        //Add an action to reverse this action.
        EventAction action = getEventAction();
        action.ID = ID;
        action.parentID = parentID;
        action.Action.type = DESTRUCTED;
        action.Action.kind = node->getGraphMLKind();
        action.Entity.kind = node->getEntityKind();
        action.Entity.nodeKind = node->getNodeKind();
        action.Entity.XML = _exportGraphMLDocument(node);
        addActionToStack(action);
    }


    if(parentNode){
        //Put this item last to fix the sort order of it's siblings.
        enforceUniqueSortOrder(node, parentNode->childrenCount());
    }

    removeGraphMLFromHash(ID);

    if(model == node){
        model = 0;
    }else if(workerDefinitions == node){
        workerDefinitions = 0;
    }
    delete node;
    return true;
}



bool NewController::destructEdge(Edge *edge)
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
        action.parentID = getModel()->getID();
        action.Action.type = DESTRUCTED;
        action.Action.kind = edge->getGraphMLKind();
        action.Entity.kind = edge->getEntityKind();
        action.Entity.XML = edge->toGraphML(0);
        action.Entity.edgeClass = edge->getEdgeClass();
        addActionToStack(action);
    }

    //Teardown specific edge classes.
    Edge::EDGE_CLASS edgeClass = edge->getEdgeClass();

    switch(edgeClass){
    case Edge::EC_DEFINITION:{
        teardownDependantRelationship(dst, src);
        break;
    }
    case Edge::EC_AGGREGATE:{
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
    case Edge::EC_ASSEMBLY:{
        // UnBind Topics Together.
        Data* sourceTopicName = src->getData("topicName");
        Data* destinationTopicName = dst->getData("topicName");
        if(destinationTopicName && sourceTopicName ){
            destinationTopicName->unsetParentData();
        }
        break;
    }
    case Edge::EC_DATA:{
        BehaviourNode* outputNode = dynamic_cast<BehaviourNode*>(src);
        BehaviourNode* inputNode = dynamic_cast<BehaviourNode*>(dst);
        if(inputNode && outputNode){
            setupDataEdgeRelationship(outputNode, inputNode, false);
        }
        break;
    }
    case Edge::EC_DEPLOYMENT:{
        if(isUserAction()){
            QString message = "Disconnected '" % src->getDataValue("label").toString() % "' from '" % dst->getDataValue("label").toString() % "'";
            emit controller_DisplayMessage(MESSAGE, message, "Deployment Changed", "Clear");
        }
    }
    default:
        break;
    }

    //Remove it from the hash of GraphML
    removeGraphMLFromHash(ID);

    delete edge;
    return true;
}

bool NewController::destructEntity(int ID)
{
    Entity* entity = getGraphMLFromID(ID);
    return destructEntity(entity);
}

bool NewController::destructEntity(Entity *item)
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



bool NewController::isEdgeLegal(Node *src, Node *dst)
{
    if(src && dst){
        //Check for dual way connections.
        return src->canConnect(dst) != Edge::EC_NONE;
    }
    return false;
}

bool NewController::isNodeKindImplemented(QString nodeKind)
{
    return containerNodeKinds.contains(nodeKind) || constructableNodeKinds.contains(nodeKind);
}

bool NewController::reverseAction(EventAction action)
{
    bool success = false;
    if(action.Action.kind == GraphML::GK_ENTITY){

        if(action.Action.type == CONSTRUCTED){
            success = destructEntity(action.ID);
        }else if(action.Action.type == DESTRUCTED){
            Node* parentNode = getNodeFromID(action.parentID);
            if(parentNode){
                success = _newImportGraphML(action.Entity.XML, parentNode);
            }
        }
    }else if(action.Action.kind == GraphML::GK_DATA){
        if(action.Action.type == CONSTRUCTED){
            Entity* entity = getGraphMLFromID(action.parentID);
            if(entity){
                success = destructData(entity, action.Data.keyName);
            }
        }else if(action.Action.type == MODIFIED){
            Entity* entity = getGraphMLFromID(action.parentID);

            if(entity){
                _setData(entity, action.Data.keyName, action.Data.oldValue);
                success = true;
            }
        }else if(action.Action.type == DESTRUCTED){
            Entity* entity = getGraphMLFromID(action.parentID);
            if(entity){
                success = _attachData(entity, action.Data.keyName, action.Data.oldValue);
            }
        }
    }else if(action.Action.kind == GraphML::GK_KEY){
        if(action.Action.type == CONSTRUCTED){
            success = destructKey(action.Key.name);
        }else if(action.Action.type == DESTRUCTED){
            success = constructKey(action.Key.name, action.Key.type, action.Key.kind);
        }
    }
    return success;
}
bool NewController::_attachData(Entity *item, QList<Data *> dataList, bool addAction)
{

    if(!item){
        return false;
    }

    bool isParameter = dynamic_cast<Parameter*>(item) != 0;

    foreach(Data* data, dataList){
        QString keyName = data->getKeyName();
        //Check if the item has a Data already.
        if(item->getData(keyName)){
            if((keyName == "x" || keyName == "y") && (data->getValue() == "" || data->getValue() == "-1")){
                //Skip bad values
                continue;
            }
            _setData(item, keyName, data->getValue(), addAction);
        }else{
            attachData(item, data, addAction);
        }

        Data* updateData = item->getData(keyName);
        if(updateData && data->isProtected()){
            updateData->setProtected(true);
        }

        if(isParameter){
            bool protect = keyName != "value";
            updateData->setProtected(protect);
        }
    }
    return true;
}

bool NewController::_attachData(Entity *item, QString keyName, QVariant value, bool addAction)
{
    Key* key = getKeyFromName(keyName);
    if(!key){
        return false;
    }

    Data* data = new Data(key);
    if(!data){
        return false;
    }

    data->setValue(value);

    return _attachData(item, data, addAction);
}

Process *NewController::getWorkerProcess(QString workerName, QString operationName)
{
    Process* process = 0;
    process = workerProcesses[workerName+"_"+operationName];
    return process;
}

bool NewController::_attachData(Entity *item, Data *data, bool addAction)
{
    QList<Data *> dataList;
    dataList.append(data);
    return _attachData(item, dataList, addAction);
}

void NewController::addActionToStack(EventAction action, bool useAction)
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

    if(USE_LOGGING){
        logAction(action);
    }
}

void NewController::undoRedo(bool undo, bool updateProgess)
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

void NewController::logAction(EventAction item)
{
    if(DESTRUCTING_CONTROLLER){
        return;
    }

    QTextStream out(logFile);

    QString actionType="";
    QString actionKind="";
    QString entityKind="";

    switch(item.Action.type){
    case CONSTRUCTED:
        actionType = "C";
        break;
    case DESTRUCTED:
        actionType = "D";
        break;
    case MODIFIED:
        actionType = "M";
        break;
    }

    switch(item.Action.kind){
    case GraphML::GK_DATA:
        actionKind = "data";
        break;
    case GraphML::GK_KEY:
        actionKind = "key";
        break;
    case GraphML::GK_ENTITY:
        actionKind = "entity";
        break;
    default:
        break;
    }

    switch(item.Entity.kind){
    case Entity::EK_EDGE:
        entityKind = "edge";
        break;
    case Entity::EK_NODE:
        entityKind = "node";
        break;
    default:
        break;
    }

    out << item.Action.ID << "\t";
    out << item.Action.actionID << "\t";
    out << actionType << "\t";
    out << actionKind << "\t";
    if(item.Entity.kind != Entity::EK_NONE){
        out << entityKind << "\t";
    }
    if(item.Action.kind != GraphML::GK_DATA){
        out << item.ID << "\t";
    }
    out << item.parentID << "\t";

    if(item.Action.kind == GraphML::GK_DATA){
        out << item.Data.keyName << "\t";
        if(item.Action.type == MODIFIED){
            out << item.Data.oldValue.toString() << "\t";
            out << "->\t";
        }
        out << item.Data.newValue.toString() << "\t";
    }
    out << "\n";
}

bool NewController::canDeleteNode(Node *node)
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
QPair<bool, QString> NewController::readFile(QString filePath)
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


void NewController::clearHistory()
{
    currentActionID = 0;
    actionCount = 0;
    currentAction = "";
    undoActionStack.clear();
    redoActionStack.clear();
    updateUndoRedoState();
}

void NewController::_projectNameChanged()
{
    Model* model = getModel();
    if(model){
        emit controller_ProjectNameChanged(model->getDataValue("label").toString());
    }
}

QString NewController::_copy(QList<Entity *> selection)
{
    QList<int> IDs = getIDs(selection);

    //Export the GraphML for those Nodes.
    QString result = _exportGraphMLDocument(IDs, false, true);

    return result;
}

Node *NewController::constructTypedNode(QString nodeKind, bool isTemporary, QString nodeType, QString nodeLabel)
{

    bool storeNode = !isTemporary;
    if(nodeKind == "Model"){
        if(model){
            return model;
        }
        return new Model();
    }else if(nodeKind == "BehaviourDefinitions"){
        if(behaviourDefinitions){
            return behaviourDefinitions;
        }
        return new BehaviourDefinitions();
    }else if(nodeKind == "InterfaceDefinitions"){
        if(interfaceDefinitions){
            return interfaceDefinitions;
        }
        return new InterfaceDefinitions();
    }else if(nodeKind == "AssemblyDefinitions"){
        if(assemblyDefinitions){
            return assemblyDefinitions;
        }
        return new AssemblyDefinitions();
    }else if(nodeKind == "HardwareDefinitions"){
        if(hardwareDefinitions){
            return hardwareDefinitions;
        }
        return new HardwareDefinitions();
    }else if(nodeKind == "DeploymentDefinitions"){
        if(deploymentDefinitions){
            return deploymentDefinitions;
        }
        return  new DeploymentDefinitions();
    }else if(nodeKind == "WorkerDefinitions"){
        if(workerDefinitions){
            return workerDefinitions;
        }
        return  new WorkerDefinitions();
    }else if(nodeKind == "HardwareNode"){
        if(hardwareEntities.contains(nodeLabel)){
            return hardwareEntities[nodeLabel];
        }else{
            HardwareNode* hN = new HardwareNode();
            if(storeNode && nodeLabel != ""){
                hardwareEntities[nodeLabel] = hN;
            }
            return hN;
        }
    }else if(nodeKind == "HardwareCluster"){
        if(hardwareEntities.contains(nodeLabel)){
            return hardwareEntities[nodeLabel];
        }else{
            HardwareCluster* hC = new HardwareCluster();
            if(storeNode && nodeLabel != ""){
                hardwareEntities[nodeLabel] = hC;
            }
            return hC;
        }
    }else if(nodeKind == "ManagementComponent"){
        if(managementComponents.contains(nodeType)){
            return managementComponents[nodeType];
        }else{
            ManagementComponent* mC = new ManagementComponent();
            if(storeNode && nodeLabel != ""){
                managementComponents[nodeLabel] = mC;
            }
            return mC;
        }
        return new ManagementComponent();
    }else if(nodeKind == "ComponentAssembly"){
        return new ComponentAssembly();
    }else if(nodeKind == "Component"){
        return new Component();
    }else if(nodeKind == "ComponentInstance"){
        return new ComponentInstance();
    }else if(nodeKind == "ComponentImpl"){
        return new ComponentImpl();
    }else if(nodeKind == "OutEventPort"){
        return new OutEventPort();
    }else if(nodeKind == "OutEventPortInstance"){
        return new OutEventPortInstance();
    }else if(nodeKind == "OutEventPortImpl"){
        return new OutEventPortImpl();
    }else if(nodeKind == "OutEventPortDelegate"){
        return new OutEventPortDelegate();
    }else if(nodeKind == "InEventPort"){
        return new InEventPort();
    }else if(nodeKind == "InEventPortInstance"){
        return new InEventPortInstance();
    }else if(nodeKind == "InEventPortImpl"){
        return new InEventPortImpl();
    }else if(nodeKind == "InEventPortDelegate"){
        return new InEventPortDelegate();
    }else if(nodeKind == "Attribute"){
        return new Attribute();
    }else if(nodeKind == "AttributeInstance"){
        return new AttributeInstance();
    }else if(nodeKind == "AttributeImpl"){
        return new AttributeImpl();
    }else if(nodeKind == "IDL"){
        return new IDL();
    }else if(nodeKind == "Member"){
        return new Member();
    }else if(nodeKind == "Aggregate"){
        return new Aggregate();
    }else if(nodeKind == "AggregateInstance"){
        return new AggregateInstance();
    }else if(nodeKind == "MemberInstance"){
        return new MemberInstance();
    }else if(nodeKind == "BranchState"){
        return new BranchState();
    }else if(nodeKind == "Condition"){
        return new Condition();
    }else if(nodeKind == "PeriodicEvent"){
        return new PeriodicEvent();
    }else if(nodeKind == "Workload"){
        return new Workload();
    }else if(nodeKind == "Process" || nodeKind == "WorkerProcess"){
        return new Process();
    }else if(nodeKind == "WhileLoop"){
        return new WhileLoop();
    }else if(nodeKind == "Termination"){
        return new Termination();
    }else if(nodeKind == "Variable"){
        return new Variable();
    }else if(nodeKind == "BlackBox"){
        return new BlackBox();
    }else if(nodeKind == "BlackBoxInstance"){
        return new BlackBoxInstance();
    }else if(nodeKind == "Vector"){
        return new Vector();
    }else if(nodeKind == "VectorInstance"){
        return new VectorInstance();
    }else if(nodeKind == "InputParameter"){
        return new InputParameter();
    }else if(nodeKind == "ReturnParameter"){
        return new ReturnParameter();
    }else if(nodeKind == "DDS_QOSProfile"){
        return new DDS_QOSProfile();
    }else if(nodeKind == "DDS_DeadlineQosPolicy"){
        return new DDS_DeadlineQosPolicy();
    }else if(nodeKind == "DDS_DestinationOrderQosPolicy"){
        return new DDS_DestinationOrderQosPolicy();
    }else if(nodeKind == "DDS_DurabilityQosPolicy"){
        return new DDS_DurabilityQosPolicy();
    }else if(nodeKind == "DDS_DurabilityServiceQosPolicy"){
        return new DDS_DurabilityServiceQosPolicy();
    }else if(nodeKind == "DDS_EntityFactoryQosPolicy"){
        return new DDS_EntityFactoryQosPolicy();
    }else if(nodeKind == "DDS_GroupDataQosPolicy"){
        return new DDS_GroupDataQosPolicy();
    }else if(nodeKind == "DDS_HistoryQosPolicy"){
        return new DDS_HistoryQosPolicy();
    }else if(nodeKind == "DDS_LatencyBudgetQosPolicy"){
        return new DDS_LatencyBudgetQosPolicy();
    }else if(nodeKind == "DDS_LifespanQosPolicy"){
        return new DDS_LifespanQosPolicy();
    }else if(nodeKind == "DDS_LivelinessQosPolicy"){
        return new DDS_LivelinessQosPolicy();
    }else if(nodeKind == "DDS_OwnershipQosPolicy"){
        return new DDS_OwnershipQosPolicy();
    }else if(nodeKind == "DDS_OwnershipStrengthQosPolicy"){
        return new DDS_OwnershipStrengthQosPolicy();
    }else if(nodeKind == "DDS_PartitionQosPolicy"){
        return new DDS_PartitionQosPolicy();
    }else if(nodeKind == "DDS_PresentationQosPolicy"){
        return new DDS_PresentationQosPolicy();
    }else if(nodeKind == "DDS_ReaderDataLifecycleQosPolicy"){
        return new DDS_ReaderDataLifecycleQosPolicy();
    }else if(nodeKind == "DDS_ReliabilityQosPolicy"){
        return new DDS_ReliabilityQosPolicy();
    }else if(nodeKind == "DDS_ResourceLimitsQosPolicy"){
        return new DDS_ResourceLimitsQosPolicy();
    }else if(nodeKind == "DDS_TimeBasedFilterQosPolicy"){
        return new DDS_TimeBasedFilterQosPolicy();
    }else if(nodeKind == "DDS_TopicDataQosPolicy"){
        return new DDS_TopicDataQosPolicy();
    }else if(nodeKind == "DDS_TransportPriorityQosPolicy"){
        return new DDS_TransportPriorityQosPolicy();
    }else if(nodeKind == "DDS_UserDataQosPolicy"){
        return new DDS_UserDataQosPolicy();
    }else if(nodeKind == "DDS_WriterDataLifecycleQosPolicy"){
        return new DDS_WriterDataLifecycleQosPolicy();
    }else{
        //qCritical() << "Node Kind:" << nodeKind << " not yet implemented!";
    }

    return 0;
}

void NewController::constructNodeGUI(Node *node)
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
    action.Entity.kind = node->getEntityKind();
    action.Entity.nodeKind = node->getNodeKind();

    if(node->getParentNode()){
        //Variable.
        Node* parentNode = node->getParentNode();
        if(parentNode && parentNode->getNodeKind() == "Variable"){
            Data* typeData = parentNode->getData("type");
            Data* childType = node->getData("type");
            typeData->setParentData(childType);
        }
    }

    //Add the node to the list of nodes constructed.
    storeGraphMLInHash(node);

    //Add Action to the Undo/Redo Stack.
    addActionToStack(action);
}

void NewController::setupModel()
{
    model = dynamic_cast<Model*>(constructTypedNode("Model"));
    _attachData(model, constructDataVector("Model"));
    constructNodeGUI(model);

    workerDefinitions = constructTypedNode("WorkerDefinitions");
    _attachData(workerDefinitions, constructDataVector("WorkerDefinitions"));
    constructNodeGUI(workerDefinitions);


    Data* labelData = model->getData("label");
    connect(labelData, SIGNAL(dataChanged(int,QString,QVariant)), this, SLOT(_projectNameChanged()));

    //Update the view with the correct Model Label.
    _projectNameChanged();

    //Construct the top level parents.
    interfaceDefinitions = constructChildNode(model, constructDataVector("InterfaceDefinitions", QPointF(-1,-1),"", "Interfaces"));
    behaviourDefinitions = constructChildNode(model, constructDataVector("BehaviourDefinitions", QPointF(-1,-1),"", "Behaviour"));
    deploymentDefinitions =  constructChildNode(model, constructDataVector("DeploymentDefinitions"));

    //Construct the second level containers.
    assemblyDefinitions =  constructChildNode(deploymentDefinitions, constructDataVector("AssemblyDefinitions", QPointF(-1,-1),"", "Assemblies"));
    hardwareDefinitions =  constructChildNode(deploymentDefinitions, constructDataVector("HardwareDefinitions", QPointF(-1,-1),"", "Hardware"));

    protectedNodes << model;
    protectedNodes << interfaceDefinitions;
    protectedNodes << behaviourDefinitions;
    protectedNodes << deploymentDefinitions;
    protectedNodes << assemblyDefinitions;
    protectedNodes << hardwareDefinitions;
    protectedNodes << workerDefinitions;


    setupManagementComponents();
    setupLocalNode();
}



void NewController::bindData(Node *definition, Node *child)
{
    if(!(definition && child)){
        return;
    }

    Data* def_Type = definition->getData("type");
    Data* def_Label = definition->getData("label");
    Data* child_Type = child->getData("type");
    Data* child_Label = child->getData("label");
    Data* def_Key = definition->getData("key");
    Data* child_Key = child->getData("key");
    Data* def_Sort = definition->getData("sortOrder");
    Data* child_Sort = child->getData("sortOrder");

    QString childKind = child->getNodeKind();
    bool bindTypes = true;
    bool bindLabels = false;
    bool bindSort = false;

    if(!definition->getDefinition() && !def_Type){
        bindTypes = false;
    }
    if((child->isInstance() || child->isImpl()) || (def_Type && def_Label)){
        if(child->getDataValue("kind") == "ComponentInstance" || child->getDataValue("kind") == "BlackBoxInstance"){
            //Allow ComponentInstance and BlackBoxInstance to have unique labels
            bindLabels = false;
        }else if(childKind == "AggregateInstance" || childKind == "VectorInstance"){
            //Allow Aggregates to contain Aggregate Instances with unique labels
            if(child->getParentNode()->getDataValue("kind") == "Aggregate"){
                bindLabels = false;
            }else{
                bindLabels = true;
            }
        }else{
            bindLabels = true;

        }
    }

    if(bindTypes){
        child_Type->setParentData(def_Type);
    }else{
        child_Type->setParentData(def_Label);
    }
    child_Type->setProtected(true);

    if(def_Key && child_Key){
        child_Key->setParentData(def_Key);
    }

    if(bindSort){
        child_Sort->setParentData(def_Sort);
    }

    if(bindLabels){
        child_Label->setParentData(def_Label);
    }else{
        //Set the value.
        if(child_Label->getValue().toString().startsWith(child->getDataValue("kind").toString())){
            if(child->isImpl()){
                _setData(child, "label", def_Label->getValue().toString() + "_Impl");
            }else{
                _setData(child, "label", def_Label->getValue().toString() + "_Inst");
            }
        }
    }
}

void NewController::unbindData(Node *definition, Node *instance)
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
bool NewController::setupDependantRelationship(Node *definition, Node *node)
{
    //Got Aggregate Edge.
    if(!(definition && node)){
        qCritical() << "setupDefinitionRelationship(): Definition or Node is NULL.";
        return false;
    }


    if(isUserAction()){
        //For each child contained in the Definition, which itself is a definition, construct an Instance/Impl inside the Parent Instance/Impl.
        foreach(Node* child, definition->getChildren(0)){
            if(child && child->isDefinition()){
                //Construct relationships between the children which matched the definitionChild.
                int instancesConnected = constructDependantRelative(node, child);

                if(instancesConnected == 0 && !node->getNodeKind().endsWith("EventPortInstance")){
                    qCritical() << "setupDefinitionRelationship(): Couldn't create a Definition Relative for: " << child->toString() << " In: " << node->toString();
                    return false;
                }
            }
        }
    }

    //Bind the un-protected Data attached to the Definition to the Instance.
    bindData(definition, node);

    //Get Connecting Edge.
    Edge* edge = node->getEdgeTo(definition);
    Key* labelKey = constructKey("label", QVariant::String, Entity::EK_EDGE);
    QString definitionType = "";

    if(!edge){
        qCritical() << "setupDefinitionRelationship(): Cannot find connecting Edge.";
        return false;
    }

    if(node->isInstance()){
        definition->addInstance(node);
        definitionType = "Instance";
    }else{
        definition->addImplementation(node);
        definitionType = "Implementation";
    }

    //Attach Data onto Edge to describe Relationship.
    if(!edge->getData(labelKey)){
        Data* label = new Data(labelKey);
        label->setValue("Is " + definitionType + " Of");
        attachData(edge, label, false);
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
bool NewController::setupEventPortAggregateRelationship(EventPort *eventPort, Aggregate *aggregate)
{
    //Got Aggregate Edge.
    if(!(eventPort && aggregate)){
        qCritical() << "setupAggregateRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    Node* aggregateInstance = 0;


    //Only auto construct if we are processing a user action.
    if(isUserAction()){
        //Check for an Existing AggregateInstance in the EventPort.
        foreach(Node* child, eventPort->getChildren(0)){
            if(child->getDataValue("kind") == "AggregateInstance"){
                aggregateInstance = child;
            }
        }

        //If we couldn't find an AggregateInstance in the EventPort, construct one.
        if(!aggregateInstance){
            aggregateInstance = constructChildNode(eventPort, constructDataVector("AggregateInstance"));
        }

        //Check that the AggregateInstance was created.
        if(!aggregateInstance){
            qCritical() << "setupAggregateRelationship(): EventPort cannot adopt an AggregateInstance!";
            return false;
        }
        //Check to see if the AggregateInstance has a Definition Yet
        if(aggregateInstance->getDefinition()){
            if(aggregateInstance->getDefinition() == aggregate){
                //qDebug() << "setupAggregateRelationship(): EventPort already contains a correctly defined AggregateInstance!";
            }else{
                qCritical() << "setupAggregateRelationship(): EventPort already contains a defined AggregateInstance!";
                return false;
            }
        }

        //Check for connecting Edge.
        Edge* edge = aggregateInstance->getEdgeTo(aggregate);

        if(!edge){
            //Construct an Edge between the AggregateInstance an Aggregate
            constructEdgeWithData(aggregateInstance, aggregate);
            edge = aggregateInstance->getEdgeTo(aggregate);
        }

        if(!edge){
            qCritical() << "setupAggregateRelationship(): Edge between AggregateInstance and Aggregate wasn't constructed!";
            return false;
        }
    }

    //Check for a connecting Edge between the eventPort and aggregate.
    Edge* edge = eventPort->getEdgeTo(aggregate);
    Key* labelKey = constructKey("label", QVariant::String, Entity::EK_EDGE);

    //Check for the existance of the Edge constructed.
    if(!edge){
        qCritical() << "setupAggregateRelationship(): Edge between EventPort and Aggregate doesn't exist!";
        return false;
    }

    //Set Label of Edge.
    if(!edge->getData(labelKey)){
        Data* label = new Data(labelKey, "Uses Aggregate");
        attachData(edge, label, false);
    }
    //Set AutoGenerated.


    //Edge Created Set Aggregate relation.
    eventPort->setAggregate(aggregate);

    //Set Type
    Data* eventPortType = eventPort->getData("type");
    Data* aggregateLabel = aggregate->getData("label");

    if(eventPortType && aggregateLabel){
        eventPortType->setParentData(aggregateLabel);
        eventPortType->setValue(aggregateLabel->getValue());
    }
    return true;
}

bool NewController::teardownEventPortAggregateRelationship(EventPort *eventPort, Aggregate *aggregate)
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

bool NewController::setupAggregateRelationship(Node *node, Aggregate *aggregate)
{
    if(!(node && aggregate)){
        qCritical() << "setupVectorRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Check for a connecting Edge between the eventPort and aggregate.
    Edge* edge = node->getEdgeTo(aggregate);


    Key* labelKey = constructKey("label", QVariant::String, Entity::EK_EDGE);

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


    EventPortDelegate* eventPortDelegate = dynamic_cast<EventPortDelegate*>(node);
    if(eventPortDelegate){
        eventPortDelegate->setAggregate(aggregate);
    }

    return true;

}

bool NewController::teardownAggregateRelationship(Node *node, Aggregate *aggregate)
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


    EventPortDelegate* eventPortDelegate = dynamic_cast<EventPortDelegate*>(node);
    if(eventPortDelegate){
        eventPortDelegate->unsetAggregate();
    }

    return true;
}

bool NewController::setupDataEdgeRelationship(BehaviourNode *output, BehaviourNode *input, bool setup)
{
    Node* inputTopParent = input->getParentNode(input->getDepthToAspect() - 2);
    Node* outputTopParent = output->getParentNode(output->getDepthToAspect() - 2);

    QString inputNodeKind;
    if(inputTopParent){
        //If we are connecting to an Variable, we don't want to bind.
        inputNodeKind = inputTopParent->getNodeKind();
        if(inputNodeKind == "Variable"){
            return true;
        }
    }


    Data* definitionData = output->getData("type");
    Data* valueData = input->getData("value");

    if(outputTopParent){
        //Bind Parent Label if we are a variable.
        QString parentNodeKind = outputTopParent->getNodeKind();
        if(parentNodeKind == "Variable" || parentNodeKind == "AttributeImpl"){
            definitionData = outputTopParent->getData("label");
        }
    }

    if(definitionData && valueData){
                /*if(setup){
                    valueData->setParentData(definitionData);
                }else{
                    valueData->unsetParentData();
                }*/
    }else{
        //return false;
    }

    //Bind special stuffs.
    Node* inputParent = input->getParentNode();
    if(inputParent){
        QString parentNodeKind = inputParent->getNodeKind();
        if(parentNodeKind == "Process"){
            QString workerName = inputParent->getDataValue("worker").toString();
            QString operationName = inputParent->getDataValue("operation").toString();
            QString parameterLabel = input->getDataValue("label").toString();

            if(workerName == "VectorOperation" && parameterLabel == "vector"){
                QStringList bindableFunctionTypes;
                bindableFunctionTypes << "get" << "set" << "remove";

                Node* bindNode = output;
                if(output->childrenCount() == 1){
                    //If this is a complex Vector Bind the child.
                    bindNode = output->getChildren(0)[0];
                }

                Data* vectorType = bindNode->getData("type");


                if(bindableFunctionTypes.contains(operationName)){
                    //Find return Parameter;
                    foreach(Node* child, inputParent->getChildren(0)){
                        Parameter* parameter = dynamic_cast<Parameter*>(child);
                        if(parameter && parameter->getDataValue("label") == "value"){
                            Data* parameterType = parameter->getData("type");
                            if(setup){
                                parameterType->setParentData(vectorType);
                            }else{
                                parameterType->unsetParentData();
                                parameterType->clearValue();
                            }
                        }
                    }
                }
            }

        }
    }


    return true;
}


bool NewController::setupParameterRelationship(Parameter *parameter, Node *data)
{
    //Get Process
    Node* parameterParent = parameter->getParentNode();


    Process* process = dynamic_cast<Process*>(parameterParent);

    if(parameter->isInputParameter()){
        Data* value = parameter->getData("value");

        QString dataKind = data->getNodeKind();
        Node* dataParent = data->getParentNode();
        if(dataKind == "VectorInstance"){
            if(dataParent->getNodeKind() == "Variable"){
                //Bind the label of the variable to the parameter.
                Data* label = dataParent->getData("label");
                value->setParentData(label);
            }
        }

        if(dataKind == "Variable"){
            //Bind the label of the variable to the parameter.
            Data* label = data->getData("label");
            Data* type = data->getData("type");
            value->setParentData(label);

            //Protect the type so that people can't change it once it's connected.
            type->setProtected(true);
        }

        if(process){
            QString workerName = process->getDataValue("worker").toString();
            QString operationName = process->getDataValue("operation").toString();
            if(workerName == "VectorOperation" && parameter->getDataValue("label") == "vector"){
                Data* bindData = data->getData("type");
                if(dataKind == "VectorInstance"){
                    if(data->childrenCount() == 1){
                        bindData = data->getChildren(0)[0]->getData("type");
                    }
                }

                QStringList bindValueParameterType;
                bindValueParameterType << "get" << "set" << "remove";
                if(bindValueParameterType.contains(operationName)){
                    //Find return Parameter;
                    foreach(Node* child, process->getChildren(0)){
                        Parameter* parameter = dynamic_cast<Parameter*>(child);
                        if(parameter && parameter->getDataValue("label") == "value"){
                            Data* returnType = parameter->getData("type");
                            returnType->setParentData(bindData);
                        }
                    }
                }
            }

        }
    }
    return true;
}

bool NewController::teardownParameterRelationship(Parameter *parameter, Node *data)
{
    if(parameter->isInputParameter()){
        QString dataKind = data->getNodeKind();
        Node* dataParent = data->getParentNode();
        if(dataKind == "VectorInstance"){
            if(dataParent->getNodeKind() == "Variable"){
                //Bind the label of the variable to the parameter.
                Data* label = dataParent->getData("label");
                label->unsetParentData();
                label->clearValue();
            }
        }
    }

    return false;

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
bool NewController:: teardownDependantRelationship(Node *definition, Node *node)
{
    //Got Aggregate Edge.
    if(!(definition && node)){
        qCritical() << "teardownDefinitionRelationship(): Definition or Node is NULL.";
        return false;
    }

    //Unbind data.
    unbindData(definition, node);

    if(!definition->gotEdgeTo(node)){
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

bool NewController::isGraphMLValid(QString inputGraphML)
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

void NewController::constructEdgeGUI(Edge *edge)
{
    //Construct an ActionItem to reverse an Edge Construction.
    EventAction action = getEventAction();

    action.Action.type = CONSTRUCTED;
    action.Action.kind = edge->getGraphMLKind();
    action.ID = edge->getID();
    action.Entity.kind = edge->getEntityKind();

    Key* descriptionKey = constructKey("description", QVariant::String, Entity::EK_EDGE);


    //Get Source and Destination of the Edge.
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    //Add Action to the Undo/Redo Stack
    addActionToStack(action);

    if(!src || !dst){
        qCritical() << "Source and Desitnation null";
    }
    Edge::EDGE_CLASS edgeClass = edge->getEdgeClass();

    switch(edgeClass){
    case Edge::EC_DEFINITION:{
        //DefinitionEdge is either an Instance or an Impl
        setupDependantRelationship(dst, src);
        break;
    }
    case Edge::EC_AGGREGATE:{
        Aggregate* aggregate = dynamic_cast<Aggregate*>(dst);
        if(aggregate){
            EventPort* eventPort = dynamic_cast<EventPort*>(src);
            if(eventPort){
                setupEventPortAggregateRelationship(eventPort, aggregate);
            }else{
                setupAggregateRelationship(src, aggregate);
            }
        }
        break;
    }
    case Edge::EC_ASSEMBLY:{
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
    case Edge::EC_DATA:{
        BehaviourNode* inputNode = dynamic_cast<BehaviourNode*>(dst);
        BehaviourNode* outputNode = dynamic_cast<BehaviourNode*>(src);
        if(inputNode && outputNode){
            setupDataEdgeRelationship(outputNode, inputNode, true);
        }
        break;
    }
    case Edge::EC_DEPLOYMENT:{
        if(isUserAction()){
            QString message = "Deployed '" % src->getDataValue("label").toString() % "' to '" % dst->getDataValue("label").toString() % "'";
            emit controller_DisplayMessage(MESSAGE, message, "Deployment Changed", "ConnectTo");
        }
        break;
    }
    case Edge::EC_QOS:{
        if(!edge->getData(descriptionKey)){
            Data* label = new Data(descriptionKey, "Using QOS");
            attachData(edge, label, false);
        }
        break;
    }
    default:
        break;
    }

    storeGraphMLInHash(edge);
}


void NewController::setupManagementComponents()
{
    //EXECUTION MANAGER
    QList<Data*> executionManagerData = constructDataVector("ManagementComponent", QPointF(-1, -1), DANCE_EXECUTION_MANAGER, DANCE_EXECUTION_MANAGER);
    QList<Data*> dancePlanLauncherData = constructDataVector("ManagementComponent", QPointF(-1, -1), DANCE_PLAN_LAUNCHER, DANCE_PLAN_LAUNCHER);
    QList<Data*> ddsLoggingServerData = constructDataVector("ManagementComponent", QPointF(-1, -1), DDS_LOGGING_SERVER, DDS_LOGGING_SERVER);
    QList<Data*> qpidBrokerData = constructDataVector("ManagementComponent", QPointF(-1, -1), QPID_BROKER, QPID_BROKER);


    protectedNodes << constructChildNode(assemblyDefinitions, executionManagerData);
    protectedNodes << constructChildNode(assemblyDefinitions, dancePlanLauncherData);
    protectedNodes << constructChildNode(assemblyDefinitions, ddsLoggingServerData);
    protectedNodes << constructChildNode(assemblyDefinitions, qpidBrokerData);
}

void NewController::setupLocalNode()
{
    //EXECUTION MANAGER
    QList<Data*> localNodeData = constructDataVector("HardwareNode") ;

    Key* localhostKey = constructKey("localhost", QVariant::Bool,Entity::EK_NODE);
    Key* readOnlyKey = constructKey("readOnly", QVariant::Bool,Entity::EK_NODE);
    Data* localhostData = new Data(localhostKey, true);
    Data* readOnlyData = new Data(readOnlyKey, true);
    localNodeData.append(localhostData);
    localNodeData.append(readOnlyData);


    foreach(Data* data, localNodeData){
        QString keyName = data->getKeyName();
        if(keyName == "label"){
            data->setValue("localhost");
        }else if(keyName == "ip_address"){
            data->setValue("127.0.0.1");
        }else if(keyName == "os"){
            data->setValue(getSysOS());
        }else if(keyName == "os_version"){
            data->setValue(getSysOSVersion());
        }else if(keyName == "architecture"){
            data->setValue(getSysArch());
        }
        data->setProtected(true);
    }

    localhostNode = constructChildNode(hardwareDefinitions, localNodeData);
    protectedNodes << localhostNode;
}

Entity*NewController::getGraphMLFromID(int ID)
{
    //Check for old IDs
    ID = getIDFromOldID(ID);

    Entity* graphML = getGraphMLFromHash(ID);
    return graphML;
}

Node *NewController::getNodeFromID(int ID)
{
    Entity* graphML = getGraphMLFromID(ID);
    return getNodeFromGraphML(graphML);
}

Node *NewController::getFirstNodeFromList(QList<int> IDs)
{
    Node* node = 0;
    foreach(int ID, IDs){
        Entity* graphML = getGraphMLFromID(ID);
        if(graphML && graphML->isNode()){
            node = (Node*)graphML;
        }
    }
    return node;
}


Edge *NewController::getEdgeFromID(int ID)
{
    Entity* graphML = getGraphMLFromID(ID);
    return getEdgeFromGraphML(graphML);
}

int NewController::getIDFromOldID(int ID)
{
    int originalID = ID;
    int newID = ID;
    while(newID != -1 ){
        if(IDLookupHash.contains(ID)){
            int temp = ID;
            ID = newID;
            newID = IDLookupHash[temp];
            if(originalID == newID){
                break;
            }
        }else{
            break;
        }
    }

    if(ID != originalID){
        //qDebug() << "Looking for ID: " <<originalID << " Found ID:" << ID;
    }
    return ID;

}

int NewController::getIntFromQString(QString ID)
{
    bool okay=true;
    int returnable = ID.toInt(&okay);
    if(okay){
        return returnable;
    }
    return -1;
}


void NewController::linkOldIDToID(int oldID, int newID)
{
    //Point the ID Hash for the oldID to the newID
    IDLookupHash[oldID] = newID;
    if(!IDLookupHash.contains(newID)){
        //Set the ID Hash for the newID to ""
        IDLookupHash[newID] = -1;
    }
}



Node *NewController::getNodeFromGraphML(Entity *item)
{
    Node* node = dynamic_cast<Node*>(item);
    return node;
}

Edge *NewController::getEdgeFromGraphML(Entity *item)
{
    Edge* edge = dynamic_cast<Edge*>(item);
    return edge;
}

bool NewController::_isInModel(Entity *item)
{
    if(model){
        return model->isAncestorOf(item);
    }else{
        return false;
    }
}

bool NewController::_isInWorkerDefinitions(Entity *item)
{
    if(workerDefinitions){
        return workerDefinitions->isAncestorOf(item);
    }else{
        return false;
    }
}

Edge *NewController::constructTypedEdge(Node *src, Node *dst, Edge::EDGE_CLASS edgeClass)
{
    Edge* returnable = 0;

    switch(edgeClass){
    case Edge::EC_DEFINITION:
        returnable = new DefinitionEdge(src, dst);
        break;
    case Edge::EC_WORKFLOW:
        returnable = new WorkflowEdge(src, dst);
        break;
    case Edge::EC_DATA:
        returnable = new DataEdge(src, dst);
        break;
    case Edge::EC_ASSEMBLY:
        returnable = new AssemblyEdge(src, dst);
        break;
    case Edge::EC_AGGREGATE:
        returnable = new AggregateEdge(src,dst);
        break;
    case Edge::EC_DEPLOYMENT:
        returnable = new DeploymentEdge(src, dst);
        break;
    case Edge::EC_QOS:
        returnable = new QOSEdge(src, dst);
        break;
    default:
        qCritical() << "CANNOT CONSTRUCT EDGE OF TYPE GG: " << src << " TO " << dst;
    }
    return returnable;
}

QString NewController::getSysOS()
{
    QString os = "undefined";

    //QTv5.5 introduced QSysInfo changes.
#if QT_VERSION >= 0x050500
    os = QSysInfo::productType();
#endif
    return os;
}

QString NewController::getSysArch()
{
    QString arch = "undefined";

    //QTv5.5 introduced QSysInfo changes.
#if QT_VERSION >= 0x050500
    arch = QSysInfo::currentCpuArchitecture();
#endif
    return arch;
}

QString NewController::getSysOSVersion()
{
    QString osv = "undefined";

    //QTv5.5 introduced QSysInfo changes.
#if QT_VERSION >= 0x050500
    osv = QSysInfo::productVersion();
#endif
    return osv;
}

QString NewController::getTimeStamp()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyyy-MM-dd hh:mm:ss");
}

uint NewController::getTimeStampEpoch()
{
    return QDateTime::currentDateTime().toTime_t();
}

Model *NewController::getModel()
{
    return model;
}

WorkerDefinitions *NewController::getWorkerDefinitions()
{
    return (WorkerDefinitions*)workerDefinitions;
}

QString NewController::getProjectAsGraphML()
{
    lock.lockForRead();
    QString data = _exportGraphMLDocument(model);
    lock.unlock();
    return data;
}

QString NewController::getSelectionAsGraphMLSnippet(QList<int> IDs)
{
    lock.lockForRead();
    QString data = _exportSnippet(IDs);
    lock.unlock();
    return data;
}

void NewController::enableDebugLogging(bool logMode, QString applicationPath)
{
    if(logMode){
        if(applicationPath != ""){
            applicationPath += "/";
        }
        QString filePath = applicationPath + "output.log";
        logFile = new QFile(filePath);

        if (!logFile->open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text)){
            QString message = "Cannot open log file: '" % filePath % "'. Logging disabled.";
            emit controller_DisplayMessage(WARNING,message, "Log Error", "Warning");
            USE_LOGGING = false;
        }else{
            USE_LOGGING = true;
        }

    }else{
        //Teardown log File.
        if(logFile){
            logFile->close();
            logFile = 0;
        }
        USE_LOGGING = false;
    }
}

/**
 * @brief NewController::displayMessage Called by the Model when a message needs to be visualised
 * Emits controller_DisplayMessage
 * @param title - The title of the message
 * @param message - The message
 * @param ID - The entity ID of which the message relates to.
 */
void NewController::displayMessage(QString title, QString message, int ID)
{
    //Emit a signal to the view.
    emit controller_DisplayMessage(MODEL, message, title, "Critical", ID);
}

/**
 * @brief NewController::setData Sets the Value of the Data of an Entity.
 * @param parentID - The ID of the Entity
 * @param keyName - The name of the Key
 * @param dataValue - The new value of the Data.
 */
void NewController::setData(int parentID, QString keyName, QVariant dataValue)
{
    Entity* graphML = getGraphMLFromID(parentID);
    if(graphML){
        _setData(graphML, keyName, dataValue, true);
    }
}

void NewController::removeData(int parentID, QString keyName)
{
    Entity* entity = getGraphMLFromID(parentID);
    if(entity){
        qCritical() << "Removing: " << parentID << " " << keyName;
        destructData(entity, keyName, true);
    }
}


void NewController::constructDestructMultipleEdges(QList<int> srcIDs, int dstID)
{

    QList<Edge*> edgesToDelete;
    bool allAlreadyDeployed = true;
    bool disconnectOnly = dstID == -1;

    Node* dst = getNodeFromID(dstID);
    //Look for destructs
    foreach(int ID, srcIDs){
        int deployedID = getDeployedHardwareID(ID);

        if(disconnectOnly || dstID != deployedID){
            allAlreadyDeployed = false;
            if(deployedID != -1){
                Node* src = getNodeFromID(ID);
                Node* hDst = getNodeFromID(deployedID);
                Edge* edge = src->getEdgeTo(hDst);
                if(edge){
                    edgesToDelete << edge;;
                }
            }
        }
    }

    QString message = "Deploying Multiple Entities to singular Hardware Entity.";
    if(disconnectOnly){
        message = "Disconnecting Multiple Entities from shared Hardware Entity";
    }
    triggerAction(message);

    //Have to delete all edges.
    if(allAlreadyDeployed){
        foreach (int srcID, srcIDs) {
            Node* src = getNodeFromID(srcID);
            if(src){
                Edge* edge =  src->getEdgeTo(dst);
                if(edge){
                    destructEdge(edge);
                }
            }
        }
    }else{
        //Destruct old deployment edges.
        while(!edgesToDelete.isEmpty()){
            Edge *edge = edgesToDelete.takeFirst();
            if(edge){
                destructEdge(edge);
            }
        }


        if(!disconnectOnly){
        //Construct new edges.
            foreach (int srcID, srcIDs) {
                Node* src = getNodeFromID(srcID);

                Edge* edge = constructEdgeWithData(src, dst);
                if(!edge){
                    //Try swap
                    constructEdgeWithData(dst, src);
                }
                if(!src || !src->gotEdgeTo(dst)){
                    QString message = "Cannot connect entity: '" % src->getDataValue("label").toString() % "'' to hardware entity: '" % dst->getDataValue("label").toString() % ".";
                    emit controller_DisplayMessage(WARNING, message, "Deployment Failed", "Failure" , srcID);
                }
            }
        }
    }

    emit controller_ActionFinished();
}


/**
 * @brief NewController::importProjects
 * @param xmlDataList
 */
void NewController::importProjects(QStringList xmlDataList)
{
    lock.lockForWrite();
    emit showProgress(true, "Importing Projects");
    IMPORTING_PROJECT = true;
    bool success = _importProjects(xmlDataList);
    IMPORTING_PROJECT = false;
    emit showProgress(false);
    lock.unlock();
    emit controller_ActionFinished(success);
}


/**
 * @brief NewController::importSnippet Imports a Snippet of GraphML into the selection defined by ID provided
 * @param IDs - The list of entity IDs
 * @param fileName - The name of the Snippet imported LABEL.<PARENT_KIND>.snippet
 * @param fileData - The graphml data of the snippet.
 */
void NewController::importSnippet(QList<int> IDs, QString fileName, QString fileData)
{
    bool success = _importSnippet(IDs, fileName, fileData);
    emit controller_ActionProgressChanged(100);
    emit controller_ActionFinished(success);
}

/**
 * @brief NewController::exportSnippet Exports a Snippet of GraphML based on the selection defined by IDs provided
 * @param IDs - The list of entity IDs
 */
void NewController::exportSnippet(QList<int> IDs)
{
    _exportSnippet(IDs);
}

/**
 * @brief NewController::gotQuestionAnswer Got an answer from the View about a question which was asked.
 * @param answer - The Boolean answer to the question
 */
void NewController::gotQuestionAnswer(bool answer)
{
    questionAnswer = answer;
    emit controller_GotQuestionAnswer();
}


void NewController::clearUndoHistory()
{
    clearHistory();
}

bool NewController::askQuestion(MESSAGE_TYPE type, QString questionTitle, QString question, int ID)
{
    if(!INITIALIZING){
        //Construct a EventLoop which waits for the View to answer the question.
        QEventLoop waitLoop;
        questionAnswer = false;

        connect(this, SIGNAL(controller_GotQuestionAnswer()), &waitLoop, SLOT(quit()));

        emit controller_AskQuestion(type, questionTitle, question, ID);

        waitLoop.exec();
        return questionAnswer;
    }
    return false;
}


Node *NewController::getSingleNode(QList<int> IDs)
{
    foreach(int ID, IDs){
        return getNodeFromID(ID);
    }
    return 0;

}

bool NewController::_newImportGraphML(QString document, Node *parent)
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
        parent = getModel();
    }

    if(!parent){
        //If we still don't have a parent. Return
        return false;
    }

    if(parent->isInstance() || parent->isImpl()){
        if(!(UNDOING || REDOING)){
            QString message =  "Cannot import/paste into an Instance.";
            emit controller_DisplayMessage(WARNING, message, "Import Error", "Warning" , parent->getID());
            return false;
        }
    }

    if(!isGraphMLValid(document)){
        emit controller_DisplayMessage(CRITICAL, "Cannot import, invalid GraphML document.", "Import Error", "Critical");
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
    TempEntity* topEntity = new TempEntity(Entity::EK_NODE);
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
        GraphML::GRAPHML_KIND currentKind = GraphML::GK_NONE;
        Entity::ENTITY_KIND currentEntityKind = Entity::EK_NONE;

        if(tagName == "edge"){
            currentKind = GraphML::GK_ENTITY;
            currentEntityKind =  Entity::EK_EDGE;
        }else if(tagName == "node"){
            currentKind = GraphML::GK_ENTITY;
            currentEntityKind = Entity::EK_NODE;
        }else if(tagName == "data"){
            currentKind = GraphML::GK_DATA;
        }else if(tagName == "key"){
            currentKind = GraphML::GK_KEY;
        }else if(tagName == "default"){
            //TODO handle import of default.
        }

        if(xml.isStartElement()){
            if(currentEntityKind != Entity::EK_NONE){
                QString ID = getXMLAttribute(xml, "id");
                int prevID = getIntFromQString(ID);


                TempEntity* entity = new TempEntity(currentEntityKind, currentEntity);
                entity->setID(ID);
                entity->setPrevID(prevID);

                entity->setLineNumber(xml.lineNumber());


                if(currentEntityKind == Entity::EK_EDGE){
                    //Handle Source/Target for edges.
                    QString srcID = getXMLAttribute(xml, "source");
                    QString dstID = getXMLAttribute(xml, "target");

                    entity->setSrcID(srcID);
                    entity->setDstID(dstID);

                    entity->setActualSrcID(getIntFromQString(srcID));
                    entity->setActualDstID(getIntFromQString(dstID));
                }

                //Insert into the hash.
                entityHash.insert(ID, entity);

                //Add to the correct stack.
                if(currentEntityKind == Entity::EK_NODE){
                    nodeIDStack.append(ID);
                }else if(currentEntityKind == Entity::EK_EDGE){
                    edgeIDStack.append(ID);
                }

                //Set the Item as the current Entity.
                currentEntity = entity;

                if(resetPosition && currentEntity && currentEntity->getParentEntity() == topEntity){
                    //Reset the position of the first item, then clear reset position.
                    currentEntity->setResetPosition();
                }
            }else if(currentKind == GraphML::GK_KEY){
                QString ID = getXMLAttribute(xml, "id");

                QString keyName = getXMLAttribute(xml, "attr.name");
                QString keyTypeStr = getXMLAttribute(xml, "attr.type");
                QString keyForStr = getXMLAttribute(xml, "for");

                QVariant::Type keyType = Key::getTypeFromGraphML(keyTypeStr);
                Entity::ENTITY_KIND keyFor = Entity::getEntityKind(keyForStr);

                Key* key = constructKey(keyName, keyType, keyFor);
                keyHash.insert(ID,key);
            }else if(currentKind == GraphML::GK_DATA){
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
            if(currentEntityKind != Entity::EK_NONE){
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
                Node* node = getNodeFromID(nodeID);
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
                            importOlder = askQuestion(CRITICAL, "Import Older Snippet", "You are trying to replace an newer version of a snippet with an older version. Would you like to proceed?", nodeID);
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
        Node* node = getNodeFromID(ID);
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
                Node* node = getNodeFromID(nodeID);
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
                    Node* parentNode = getNodeFromID(parentEntity->getActualID());

                    Node* newNode = 0;

                        //Don't attach model information for anything but Open
                    if(!OPENING_PROJECT && entity->getNodeKind() == "Model"){
                        newNode = getModel();
                        //Ignore the construction.
                        entity->setIgnoreConstruction();
                    }else{
                        QList<Data*> dataList = entity->takeDataList();

                        newNode = _constructNode(dataList);
                    }

                    if(!newNode){
                        QString message = "Cannot create Node '" % entity->getNodeKind() % "' from document at line #" % QString::number(entity->getLineNumber()) % ".";
                        emit  controller_DisplayMessage(WARNING, message, "Import Error", "Import");
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
                        linkOldIDToID(entity->getPrevID(), nodeID);
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


    QMap<Edge::EDGE_CLASS, TempEntity*> edgesMap;
    //Order Edges into map.
    while(!edgeIDStack.isEmpty()){
        //Get the String ID of the node.
        QString ID = edgeIDStack.takeFirst();
        TempEntity *entity = entityHash[ID];

        if(entity && entity->isEdge()){
            TempEntity* srcEntity = entityHash[entity->getSrcID()];
            TempEntity* dstEntity = entityHash[entity->getDstID()];

            Node* src = 0;
            Node* dst = 0;

            if(srcEntity && srcEntity->gotActualID()){
                src = getNodeFromID(srcEntity->getActualID());
            }else if(entity->getActualSrcID() > 0){
                src = getNodeFromID(entity->getActualSrcID());
            }

            if(dstEntity && dstEntity->gotActualID()){
                dst = getNodeFromID(dstEntity->getActualID());
            }else if(entity->getActualDstID() > 0){
                dst = getNodeFromID(entity->getActualDstID());
            }

            if(src && dst){
                bool inMap = false;
                if(dst->isDefinition()){
                    if(src->isInstance() || src->isImpl()){
                        edgesMap.insertMulti(Edge::EC_DEFINITION, entity);
                        inMap = true;
                    }else if(dst->getNodeKind() == "Aggregate"){
                        edgesMap.insertMulti(Edge::EC_AGGREGATE, entity);
                        inMap = true;
                    }
                }
                if(!inMap){
                    edgesMap.insertMulti(Edge::EC_UNDEFINED, entity);
                }
            }else{
                //Don't construct if we have an error.
				entity->setIgnoreConstruction();
                emit  controller_DisplayMessage(WARNING, "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ".", "Import Error", "Import");
			}
        }
    }

    if(updateProgressNotification()){
        emit showProgress(true, "Constructing Edges");
    }

    QList<Edge::EDGE_CLASS> edgeOrder;
    edgeOrder << Edge::EC_DEFINITION << Edge::EC_AGGREGATE << Edge::EC_UNDEFINED;

    foreach(Edge::EDGE_CLASS edgeClass, edgeOrder){
        QList<TempEntity*> entityList = edgesMap.values(edgeClass);

        while(!entityList.isEmpty()){
            TempEntity* entity = entityList.takeFirst();
            if(entity){
                TempEntity* srcEntity = entityHash[entity->getSrcID()];
                TempEntity* dstEntity = entityHash[entity->getDstID()];

                Node* src = 0;
                Node* dst = 0;

                if(srcEntity && srcEntity->gotActualID()){
                    src = getNodeFromID(srcEntity->getActualID());
                }else if(entity->getActualSrcID() > 0){
                    src = getNodeFromID(entity->getActualSrcID());
                }

                if(dstEntity && dstEntity->gotActualID()){
                    dst = getNodeFromID(dstEntity->getActualID());
                }else if(entity->getActualDstID() > 0){
                    dst = getNodeFromID(entity->getActualDstID());
                }

                if(src && dst){
                    Edge* edge = src->getEdgeTo(dst);
                    if(edge){
                        _attachData(edge, entity->takeDataList());
                    }else{
                        edge = constructEdgeWithData(src, dst, entity->takeDataList());
                    }

                    if(edge){
                        if(updateProgressNotification()){
                            emit progressChanged((entitiesMade* 100) / totalEntities);
                        }
                        entitiesMade ++;

                        if(linkPreviousID && entity->hasPrevID()){
                            //Link the old ID
                            linkOldIDToID(entity->getPrevID(), edge->getID());
                        }
                    }else{
                        if(entity->getRetryCount() < 3){
                            entity->incrementRetryCount();
                            entityList.append(entity);
                        }
                    }
                }
            }
        }
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

ReadOnlyState NewController::getReadOnlyState(Node *node)
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

EventAction NewController::getEventAction()
{
    EventAction action;
    action.projectDirty = projectDirty;
    action.Action.ID = currentActionID;
    action.Action.name = currentAction;
    action.Action.actionID = currentActionItemID++;
    return action;
}

void NewController::setProjectDirty(bool dirty)
{
    if(projectDirty != dirty){
        projectDirty = dirty;
        emit projectModified(dirty);
    }
}

void NewController::setProjectPath(QString path)
{
    if(projectPath != path){
        projectPath = path;
        emit controller_ProjectFileChanged(projectPath);
    }
}


bool NewController::canCopy(QList<int> IDs)
{
    Node* parent = 0;
    if(IDs.length() == 0){
        return false;
    }
    bool gotNode = false;

    foreach(int ID, IDs){
        Node* node = getNodeFromID(ID);

        if(!node){
            //Probably an Edge!
            continue;
        }
        gotNode = true;
        if(!canDeleteNode(node)){
            return false;
        }
        if(!parent){
            //Set the firstParent to the first Nodes parent.
            parent = node->getParentNode();
        }

        if(node->getParentNode() != parent){
            //controller_DisplayMessage(WARNING, "Error", "Can only copy or cut entities which share the same parent.", ID);
            return false;
        }
    }
    return gotNode;
}

bool NewController::canGetCPP(QList<int> IDs)
{
    if(IDs.length() != 1){
        return false;
    }

    foreach(int ID, IDs){
        Node* node = getNodeFromID(ID);
        ComponentImpl* componentImpl = dynamic_cast<ComponentImpl*>(node);
        if(componentImpl){
            return true;
        }
    }
    return false;
}

bool NewController::canReplicate(QList<int> selection)
{
    //Uses can cut for the moment to try catch replication of implemenations.
    if(!canCut(selection)){
        return false;
    }

    //Find selections parent, to see if paste would work
    Node* parentNode = 0;

    foreach(int ID, selection){
        Node* node = getNodeFromID(ID);

        if(!node){
            //Probably an Edge!
            continue;
        }
        if(node->getParentNode()){
            if(!parentNode){
                parentNode = node->getParentNode();
            }
            if(parentNode != node->getParentNode()){
                return false;
            }
        }
    }

    if(parentNode){
        QList<int> parentID;
        parentID << parentNode->getID();
        if(!canPaste(parentID)){
            return false;
        }
    }
    return true;
}

bool NewController::canCut(QList<int> selection)
{
    return canCopy(selection) && canDelete(selection);
}

bool NewController::canReplicate(QList<Entity *> selection)
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

bool NewController::canCut(QList<Entity *> selection)
{
    return canCopy(selection) && canDelete(selection);
}

bool NewController::canCopy(QList<Entity *> selection)
{
    Node* parent = 0;

    bool valid = !selection.isEmpty();

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

bool NewController::canPaste(QList<Entity *> selection)
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

bool NewController::canDelete(QList<Entity *> selection)
{
    if(selection.isEmpty()){
        return false;
    }

    foreach(Entity* entity, selection){
        Node* node = 0;

        if(entity->isNode()){
            node = (Node*) entity;
        }

        if(node){
            if(!canDeleteNode(node)){
                return false;
            }

            if(node->getParentNode()){
                Parameter* pNode = dynamic_cast<Parameter*>(node);
                Variable* vNode = dynamic_cast<Variable*>(node->getParentNode());
                if(pNode){
                    return false;
                }
                if(vNode && node->isInstance()){
                    //Can't Instance things inside Variables!
                    return false;
                }
                if(node->isImpl() && node->getDefinition()){
                    if(node->getDataValue("kind") != "OutEventPortImpl"){
                        return false;
                    }
                }

                if(node->isInstance() && node->getParentNode()->isInstance()){
                    return false;
                }

                if(node->isReadOnly()){
                    if(node->getParentNode()->isReadOnly()){
                        return false;
                    }
                }
            }
        }
    }
    return true;



}

bool NewController::canDelete(QList<int> selection)
{
    if(selection.size() == 0){
        return false;
    }
    foreach(int ID, selection){
        Node* node = getNodeFromID(ID);
        if(!node){
            continue;
        }
        if(!canDeleteNode(node)){
            return false;
        }
        if(node->getParentNode()){
            Parameter* pNode = dynamic_cast<Parameter*>(node);
            Variable* vNode = dynamic_cast<Variable*>(node->getParentNode());
            if(pNode){
                return false;
            }
            if(vNode && node->isInstance()){
                //Can't Instance things inside Variables!
                return false;
            }
            if(node->isImpl() && node->getDefinition()){
                if(node->getDataValue("kind") != "OutEventPortImpl"){
                    return false;
                }
            }

            if(node->isInstance() && node->getParentNode()->isInstance()){
                return false;
            }

            if(node->isReadOnly()){
                if(node->getParentNode()->isReadOnly()){
                    return false;
                }
            }
        }

    }
    return true;
}

bool NewController::canPaste(QList<int> selection)
{
    if(selection.size() == 1){
        Entity* graphml = getGraphMLFromID(selection[0]);\
        if(graphml && graphml->isNode() && graphml != model){
            Node* node = (Node*)graphml;
            if(node->isReadOnly()){
                return false;
            }
            if(node->isInstance() || node->isImpl()){
                return false;
            }
            return true;
        }
    }
    return false;
}

bool NewController::canExportSnippet(QList<int> IDs)
{
    Node* parent = 0;
    if(IDs.length() == 0){
        return false;
    }

    foreach(int ID, IDs){
        Node* node = getNodeFromID(ID);
        Edge* edge = getEdgeFromID(ID);
        if(!node && !edge){
            return false;
        }
        if(edge){
            continue;
        }
        if(nonSnippetableKinds.contains(node->getDataValue("kind").toString())){
            return false;
        }
        Data* readOnlyData = node->getData("readOnly");
        if(readOnlyData && readOnlyData->getValue().toBool()){
            //Can't Export Read-Only Stuffs.
            return false;
        }
        if(!parent){
            //Set the firstParent to the first Nodes parent.
            parent = node->getParentNode();
            if(!parent){
                return false;
            }
            if(!snippetableParentKinds.contains(parent->getDataValue("kind").toString())){
                return false;
            }
        }else if(node->getParentNode() != parent){
            return false;
        }
    }
    return true;
}

bool NewController::canImportSnippet(QList<int> selection)
{
    if(selection.length() != 1){
        return false;
    }
    Node* parent = getNodeFromID(selection[0]);
    if(!parent){
        return false;
    }

    if(snippetableParentKinds.contains(parent->getDataValue("kind").toString())){
        return true;
    }
    return false;
}

bool NewController::canSetReadOnly(QList<int> IDs)
{
    bool gotAnyNonReadOnly=false;
    foreach(int ID, IDs){
        Entity* entity = getGraphMLFromID(ID);
        if(entity){
            if(!entity->isReadOnly()){
                gotAnyNonReadOnly = true;
                break;
            }
        }
    }
    return gotAnyNonReadOnly;
}

bool NewController::canUnsetReadOnly(QList<int> IDs)
{
    bool gotAnyReadOnly=false;
    foreach(int ID, IDs){
        Entity* entity = getGraphMLFromID(ID);
        if(entity){
            if(entity->isReadOnly()){
                gotAnyReadOnly = true;
                break;
            }
        }
    }
    return gotAnyReadOnly;
}

bool NewController::canUndo()
{
    return !undoActionStack.isEmpty();
}

bool NewController::canRedo()
{

    return !redoActionStack.isEmpty();
}

/**
 * @brief NewController::canLocalDeploy Checks to see if Model only contains deployment edges to the local node.
 * @return
 */
bool NewController::canLocalDeploy()
{
    if(!assemblyDefinitions){
        return false;
    }
    bool isDeployable = false;

    //Check to see if all nodes in the assembly definitions are deployed to the localhost node.
    foreach(Node* node, assemblyDefinitions->getChildren()){
        foreach(Edge* edge, node->getEdges(0)){
            if(edge->isDeploymentLink()){
                if(!edge->contains(localhostNode)){
                    return false;
                }else{
                    isDeployable = true;
                }
                break;
            }
        }
    }
    return isDeployable;
}

QString NewController::getProjectPath() const
{
    return projectPath;
}



bool NewController::isProjectSaved() const
{
    return projectDirty;
}


int NewController::getDefinition(int ID)
{
    Node* original = getNodeFromID(ID);
    if(original){
        Node* node = original->getDefinition(true);
        if(node && node != original){
            return node->getID();
        }
    }
    return -1;
}

int NewController::getImplementation(int ID)
{
    Node* original = getNodeFromID(ID);
    if(original){
        int definitionID = getDefinition(ID);
        if(definitionID == -1){
            definitionID = ID;
        }
        Node* node = getNodeFromID(definitionID);

        if (node && node->getImplementations().size() > 0) {
            Node* impl = node->getImplementations().at(0);
            if (impl != original) {
                return impl->getID();
            }
        }
    }
    return -1;
}

Node* NewController::getSharedParent(QList<int> IDs)
{
    Node* parent = 0;

    foreach(int ID, IDs){
        Node* node = getNodeFromID(ID);
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

QString NewController::getDataValueFromKeyName(QList<Data *> dataList, QString keyName)
{
    foreach(Data* data, dataList){
        if(data && data->getKeyName() == keyName){
            return data->getValue().toString();
        }
    }
    return "";
}

void NewController::setDataValueFromKeyName(QList<Data *> dataList, QString keyName, QString value)
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
QString NewController::getProcessName(Process *process)
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


bool NewController::isUserAction()
{
    if(UNDOING || REDOING || OPENING_PROJECT || IMPORTING_PROJECT || INITIALIZING || PASTE_USED || DESTRUCTING_CONTROLLER){
        return false;
    }else{
        return true;
    }
}


QDataStream &operator<<(QDataStream &out, const EventAction &a)
{
    //Serialize Action output.
    out << a.Action.ID << a.Action.actionID << a.Action.type << a.Action.kind << a.Action.name << a.Action.timestamp;
    return out;

}
