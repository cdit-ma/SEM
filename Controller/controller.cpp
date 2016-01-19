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

#include "edgeadapter.h"
#include "nodeadapter.h"

bool UNDO = true;
bool REDO = false;
bool SETUP_AS_INSTANCE = true;
bool SETUP_AS_IMPL = false;

NewController::NewController()
{
    qRegisterMetaType<MESSAGE_TYPE>("MESSAGE_TYPE");
    qRegisterMetaType<GraphML::GRAPHML_KIND>("GraphML::GRAPHML_KIND");
    qRegisterMetaType<QList<int> >("QList<int>");

    Model::resetID();


    logFile = 0;

    PASTE_USED = false;
    IMPORTING_PROJECT = false;
    USE_LOGGING = false;
    UNDOING = false;
    OPENING_PROJECT = false;
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
    protectedKeyNames << "kind";

    visualKeyNames << "x" << "y" << "width" << "height" << "isExpanded" << "readOnly";


    containerNodeKinds << "Model";
    containerNodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    containerNodeKinds << "HardwareDefinitions" << "AssemblyDefinitions" << "ManagementComponent";
    containerNodeKinds << "HardwareCluster";
    containerNodeKinds << "WorkerDefinitions";

    definitionNodeKinds << "IDL" << "Component" << "Attribute" << "ComponentAssembly" << "ComponentInstance" << "BlackBox" << "BlackBoxInstance";
    definitionNodeKinds << "Member" << "Aggregate";
    definitionNodeKinds << "InEventPort"  << "OutEventPort";
    definitionNodeKinds << "InEventPortDelegate"  << "OutEventPortDelegate";
    definitionNodeKinds << "AggregateInstance";
    definitionNodeKinds << "ComponentImpl";

    definitionNodeKinds << "Vector" << "VectorInstance";








    behaviourNodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload" << "OutEventPortImpl";
    behaviourNodeKinds << "WhileLoop" << "InputParameter" << "ReturnParameter" << "AggregateInstance" << "VectorInstance";


    //Append Kinds which can't be constructed by the GUI.
    constructableNodeKinds << "MemberInstance" << "AttributeImpl";
    constructableNodeKinds << "OutEventPortInstance" << "MemberInstance" << "AggregateInstance";
    constructableNodeKinds << "AttributeInstance" << "AttributeImpl";
    constructableNodeKinds << "InEventPortInstance" << "InEventPortImpl";
    constructableNodeKinds << "OutEventPortInstance" << "OutEventPortImpl" << "HardwareNode";

    snippetableParentKinds << "ComponentImpl" << "InterfaceDefinitions";
    nonSnippetableKinds << "OutEventPortImpl" << "InEventPortImpl";

    constructableNodeKinds.append(definitionNodeKinds);
    constructableNodeKinds.append(behaviourNodeKinds);
    constructableNodeKinds << "ManagementComponent";

    constructableNodeKinds << "InputParameter";

    constructableNodeKinds.removeDuplicates();

    guiConstructableNodeKinds.append(definitionNodeKinds);
    guiConstructableNodeKinds.append(behaviourNodeKinds);
    guiConstructableNodeKinds.removeDuplicates();
    guiConstructableNodeKinds.sort();

}

void NewController::connectView(NodeView *view)
{
    view->setController(this);

    //Connect SIGNALS to view Slots (ALL VIEWS)
    connect(this, SIGNAL(controller_EntityConstructed(EntityAdapter*)), view, SLOT(constructEntityItem(EntityAdapter*)));
    connect(this, SIGNAL(controller_EntityDestructed(EntityAdapter*)), view, SLOT(destructEntityItem(EntityAdapter*)));


    connect(this, SIGNAL(controller_SetViewEnabled(bool)), view, SLOT(setEnabled(bool)));



    if(view->isMainView()){
        connect(this, SIGNAL(controller_ProjectFileChanged(QString)), view, SIGNAL(view_ProjectFileChanged(QString)));
        connect(this, SIGNAL(controller_ProjectNameChanged(QString)), view, SIGNAL(view_ProjectNameChanged(QString)));


        connect(view, SIGNAL(view_ConstructWorkerProcessNode(int,QString,QString,QPointF)), this, SLOT(constructWorkerProcessNode(int, QString, QString, QPointF)));
        connect(this, SIGNAL(controller_CanRedo(bool)), view, SLOT(canRedo(bool)));
        connect(this, SIGNAL(controller_CanUndo(bool)), view, SLOT(canUndo(bool)));
        connect(view, SIGNAL(view_constructDestructEdges(QList<int>, int)), this, SLOT(constructDestructMultipleEdges(QList<int>, int)));
        connect(view, SIGNAL(view_EnableDebugLogging(bool, QString)), this, SLOT(enableDebugLogging(bool, QString)));
        connect(this, SIGNAL(controller_ProjectRequiresSave(bool)), view, SIGNAL(view_ProjectRequiresSaving(bool)));


        connect(view, SIGNAL(view_DestructEdge(int, int)), this, SLOT(destructEdge(int, int)));
        //Pass Through Signals to GUI.
        connect(view, SIGNAL(view_ClearHistoryStates()), this, SLOT(clearHistory()));
        connect(view, SIGNAL(view_Clear()), this, SLOT(clear()));
        connect(this, SIGNAL(controller_ExportedProject(QString)), view, SIGNAL(view_ExportedProject(QString)));

        connect(this, SIGNAL(controller_SetClipboardBuffer(QString)), view, SIGNAL(view_SetClipboardBuffer(QString)));


        connect(this, SIGNAL(controller_ActionProgressChanged(int,QString)), view, SIGNAL(view_updateProgressStatus(int,QString)));

        //Signals to the View.
        connect(this, SIGNAL(controller_DisplayMessage(MESSAGE_TYPE, QString, QString, int)), view, SLOT(showMessage(MESSAGE_TYPE,QString,QString,int)));

        // Re-added this for now

        connect(this, SIGNAL(controller_ExportedSnippet(QString,QString)), view, SIGNAL(view_ExportedSnippet(QString,QString)));

        connect(this, SIGNAL(controller_AskQuestion(MESSAGE_TYPE, QString, QString, int)), view, SLOT(showQuestion(MESSAGE_TYPE,QString,QString,int)));
        connect(view, SIGNAL(view_QuestionAnswered(bool)), this, SLOT(gotQuestionAnswer(bool)));
        connect(this, SIGNAL(controller_ModelReady()), view, SLOT(modelReady()));
    }

    if(view->isMainView()){
        connect(this, SIGNAL(controller_ActionFinished()), view, SLOT(actionFinished()));
        connect(view, SIGNAL(view_Replicate(QList<int>)), this, SLOT(replicate(QList<int>)));
        //File SLOTS
        connect(view, SIGNAL(view_ImportProjects(QStringList)), this, SLOT(importProjects(QStringList)));

        connect(view, SIGNAL(view_OpenProject(QString, QString)), this, SLOT(openProject(QString, QString)));
        connect(view, SIGNAL(view_SaveProject(QString)), this, SLOT(saveProject(QString)));
        connect(this, SIGNAL(controller_SavedProject(QString,QString)), view, SIGNAL(view_SavedProject(QString, QString)));
        connect(view, SIGNAL(view_ProjectSaved(bool,QString)), this, SLOT(projectSaved(bool,QString)));

        connect(view, SIGNAL(view_ImportedSnippet(QList<int>,QString,QString)), this, SLOT(importSnippet(QList<int>,QString,QString)));
        connect(view, SIGNAL(view_ExportSnippet(QList<int>)), this, SLOT(exportSnippet(QList<int>)));

        //Edit SLOTS
        connect(view, SIGNAL(view_Undo()), this, SLOT(undo()));
        connect(view, SIGNAL(view_Redo()), this, SLOT(redo()));
        connect(view, SIGNAL(view_Delete(QList<int>)), this, SLOT(remove(QList<int>)));
        connect(view, SIGNAL(view_Copy(QList<int>)), this, SLOT(copy(QList<int>)));
        connect(view, SIGNAL(view_Cut(QList<int>)), this, SLOT(cut(QList<int>)));
        connect(view, SIGNAL(view_SetReadOnly(QList<int>,bool)), this, SLOT(setReadOnly(QList<int>,bool)));
        connect(view, SIGNAL(view_Paste(int,QString)), this, SLOT(paste(int,QString)));

        //Node Slots
        connect(view, SIGNAL(view_ConstructEdge(int,int, bool)), this, SLOT(constructEdge(int, int, bool)));
        connect(view, SIGNAL(view_ConstructNode(int,QString,QPointF)), this, SLOT(constructNode(int,QString,QPointF)));


        connect(view, SIGNAL(view_ConstructConnectedNode(int,int,QString,QPointF)), this, SLOT(constructConnectedNode(int,int,QString,QPointF)));

        //Undo SLOTS
        connect(view, SIGNAL(view_TriggerAction(QString)), this, SLOT(triggerAction(QString)));
        connect(view, SIGNAL(view_SetData(int, QString, QVariant)), this, SLOT(setData(int, QString, QVariant)));


    }
}



void NewController::initializeModel()
{
    setupModel();
    loadWorkerDefinitions();
    clearHistory();
    INITIALIZING = false;
    emit controller_ModelReady();
}

NewController::~NewController()
{
    enableDebugLogging(false);

    DESTRUCTING_CONTROLLER = true;

    destructNode(model);
    destructNode(workerDefinitions);
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

        QStringList fileExtension("*.worker.graphml");
        foreach(QDir directory, workerDirectories){
            //Foreach *.worker.graphml file in the workerDefPath, load the graphml.
            foreach(QString fileName, directory.entryList(fileExtension)){
                if(fileName == "VariableOperation.worker.graphml"){
                    //Ignore VariableOperation
                    continue;
                }

                QString importFileName = directory.absolutePath() + "/" + fileName;

                QPair<bool, QString> data = readFile(importFileName);
                //If the file was read.
                if(data.first){
                    bool success = _importGraphMLXML(data.second, workerDefinition, false, true);
                    if(!success){
                        emit controller_DisplayMessage(WARNING, "Cannot Import worker definition", "MEDEA cannot import worker definition'" + importFileName +"'!");
                    }else{
                        qCritical() << "Loaded Worker Definition: " << importFileName;
                    }
                }else{
                     emit controller_DisplayMessage(WARNING, "Cannot read worker definition", "MEDEA cannot read worker definition'" + importFileName +"'!");
                }
            }
        }


        foreach(Node* child, workerDefinition->getChildren()){
            Process* process = dynamic_cast<Process*>(child);
            QString longName = getProcessName(process);
            if(longName != ""){
                if(!workerProcesses.contains(longName)){
                    workerProcesses[longName] = process;
                }else{
                    emit controller_DisplayMessage(WARNING, "Duplicate Worker Definitions", "MEDEA has found 2 worker operations with the same name! Using the first found.");
                }
            }
        }
    }

    //Once we have loaded in workers, we should keep a dictionary lookup for them.
}

QString NewController::_exportGraphMLDocument(QList<int> nodeIDs, bool allEdges, bool GUI_USED)
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
    bool informQuestion = false;
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
                        controller_DisplayMessage(MESSAGE, "", "", src->getID());
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
        nodeXML += node->toGraphML(2);

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

QStringList NewController::getViewAspects()
{
    return viewAspects;
}

QStringList NewController::getAllNodeKinds()
{
    return constructableNodeKinds;
}

QStringList NewController::getGUIConstructableNodeKinds()
{
    return guiConstructableNodeKinds;
}




bool NewController::_clear()
{
    bool reply = askQuestion(CRITICAL, "Clear Model?", "Are you sure you want to clear the model? You cannot undo this action.");
    if(reply){
        triggerAction("Clearing Model");

        emit controller_ActionProgressChanged(0,"Clearing Model");
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



void NewController::setData(Entity *parent, QString keyName, QVariant dataValue, bool addAction)
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

    if(data){
        action.ID = data->getID();
        action.Data.oldValue = data->getValue();

        if(dataValue == action.Data.oldValue){
            //Don't update if we have got the same value in the model.
            return;
        }

        if(keyName == "label"){
            if(parent->isNode()){
                enforceUniqueLabel((Node*)parent, dataValue.toString());
            }
        }else if(keyName == "sortOrder"){
            if(parent->isNode()){
                enforceUniqueSortOrder((Node*)parent, dataValue.toInt());
            }
            data->setValue(dataValue);
        }else{
            data->setValue(dataValue);
        }
        action.Data.newValue = data->getValue();

    }else{
        qCritical() << "view_UpdateData() Doesn't Contain Data for Key: " << keyName;
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
            foreach(Node* child, parentNode->getChildrenOfKind("InputParameter", 0)){
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

void NewController::constructWorkerProcessNode(int parentID, QString workerName, QString operationName, QPointF position)
{
    Node* parentNode = getNodeFromID(parentID);

    Process* processDefinition = getWorkerProcess(workerName, operationName);


    triggerAction("Constructing worker Process");
    Node* processFunction = cloneNode(processDefinition, parentNode);

    emit controller_ActionFinished();
    return;
}

void NewController::constructEdge(int srcID, int dstID, bool reverseOkay)
{
    Node* src = getNodeFromID(srcID);
    Node* dst = getNodeFromID(dstID);
    Data* label = src->getData("label");
    if(src && dst){
        Edge* edge = constructEdgeWithData(src, dst);
        if(!edge){
            //Try swap
            constructEdgeWithData(dst, src);
        }
    }
    emit controller_ActionFinished();
}

void NewController::destructEdge(int srcID, int dstID)
{
    Node* src = getNodeFromID(srcID);
    Node* dst = getNodeFromID(dstID);

    if(src && dst){
        Edge* edge = src->getEdgeTo(dst);
        destructEdge(edge);
    }
    emit controller_ActionFinished();
}

void NewController::constructConnectedNode(int parentID, int connectedID, QString kind, QPointF relativePos)
{
    Node* parentNode = getNodeFromID(parentID);
    Node* connectedNode = getNodeFromID(connectedID);
    if(parentNode && connectedNode){
        //Disable the auto send of construct graphml items.
        setViewSignalsEnabled(false);
        triggerAction("Constructed Connected Node");
        Node* newNode = constructChildNode(parentNode, constructDataVector(kind));
        //bool gotEdge = false;
        if(newNode){
            //Update the position
            setData(newNode, "x", relativePos.x());
            setData(newNode, "y", relativePos.y());

            constructEdgeWithData(newNode, connectedNode);

            //Try the alternate connection.
            if(!newNode->gotEdgeTo(newNode)){
                constructEdgeWithData(connectedNode, newNode);
            }
        }

        Edge* connectingEdge = newNode->getEdgeTo(connectedNode);

        if(!connectingEdge){
            _remove(newNode->getID(),false);
            viewSignalsList.clear();
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
        undoRedo(UNDO);
    }
    emit controller_ActionFinished();
}

void NewController::redo()
{
    if(canRedo()){
        undoRedo(REDO);
    }
    emit controller_ActionFinished();
}

void NewController::saveProject(QString filePath="")
{
    if(model){
        QString fileData = _exportGraphMLDocument(model);

        if(filePath == ""){
            //Use current project file path.
            filePath = projectFileSavePath;
        }
        emit controller_SavedProject(filePath, fileData);
    }
    emit controller_ActionFinished();
}

void NewController::openProject(QString filePath, QString xmlData)
{
    OPENING_PROJECT = true;

    bool result = _importGraphMLXML(xmlData, getModel());
    if(!result){
        emit controller_ActionProgressChanged(100);
        controller_DisplayMessage(CRITICAL, "Open Error", "Cannot fully open document.", getModel()->getID());
        //Undo the failed load.
        //undoRedo(true);
    }
    setProjectFilePath(filePath);

    //Clear the Undo/Redo History.
    clearHistory();

    OPENING_PROJECT = false;

    //Loading a project means we are in state with the savefile.
    setProjectDirty(false);

    emit controller_ActionFinished();
}


/**
 * @brief NewController::copy - Attempts to copy a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void NewController::copy(QList<int> IDs)
{
    _copy(IDs);
    emit controller_ActionFinished();
}

/**
 * @brief NewController::remove - Attempts to remove a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
void NewController::remove(QList<int> IDs)
{
    if(canDelete(IDs)){
        if(!_remove(IDs)){
            controller_DisplayMessage(WARNING, "Delete Error", "Cannot delete all selected entities.");
            emit controller_ActionProgressChanged(100);
        }
    }
    emit controller_ActionFinished();
}

void NewController::setReadOnly(QList<int> IDs, bool readOnly)
{
    qCritical() << IDs;
    qCritical() << "SETTING READ ONLY" << readOnly;
    QString exportTimeStamp = getTimeStamp();

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

    //Attach read only Data.
    foreach(Node* node, nodeList){
        Data* readOnlyData = node->getData(readOnlyKey);

        if(!readOnlyData){
            readOnlyData = new Data(readOnlyKey, readOnly);
            attachData(node, readOnlyData);
        }else{
            setData(node, "readOnly", readOnly);
        }
    }



    emit controller_ActionFinished();
}

/**
 * @brief NewController::clear
 */
void NewController::clear()
{
    if(!_clear()){
        emit controller_ActionProgressChanged(100);
    }
    emit controller_ActionFinished();
}

/**
 * @brief NewController::replicate Essentially copies and pastes the ID's in place.
 * @param IDs - The list of entity IDs
 */
void NewController::replicate(QList<int> IDs)
{
    if(!_replicate(IDs)){
        emit controller_ActionProgressChanged(100);
    }
    emit controller_ActionFinished();
}

/**
 * @brief NewController::cut Copies a selection of IDs and then deletes them.
 * @param selectedIDs - The list of entity IDs
 */
void NewController::cut(QList<int> IDs)
{
    if(!_cut(IDs)){
        emit controller_ActionProgressChanged(100);
    }
    emit controller_ActionFinished();
}



/**
 * @brief NewController::paste Tells the Controller to Paste
 * @param ID - The ID of the node to paste into
 * @param xmlData - The GraphML Data to paste.
 */
void NewController::paste(int ID, QString xmlData)
{
    if(!_paste(ID, xmlData)){
        emit controller_ActionProgressChanged(100);
    }

    emit controller_ActionFinished();
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
        controller_DisplayMessage(WARNING, "Paste" ,"Please select an entity to paste into.");
        success = false;
    }else{
        if(isGraphMLValid(xmlData) && xmlData != ""){
            PASTE_USED = true;
            if(addAction){
                triggerAction("Pasting Selection.");
                emit controller_ActionProgressChanged(0, "Pasting Selection");
            }

            //Paste it into the current Selected Node,
            success = _importGraphMLXML(xmlData, parentNode, CUT_USED, true);
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
            emit controller_ActionProgressChanged(0, "Cutting Selection");
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
        controller_SetClipboardBuffer(result);

        success = true;
    } else {
        emit controller_DisplayMessage(WARNING, "Error", "Cannot copy/cut selection.");
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
            emit controller_ActionProgressChanged(0,"Removing Selection");
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

bool NewController::_remove(int ID, bool addAction)
{
    QList<int> IDs;
    IDs << ID;
    return _remove(IDs, addAction);
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

        Node* node = getFirstNodeFromList(IDs);
        if(node && node->getParentNode()){
            emit controller_ActionProgressChanged(0,"Replicating Selection");
            //Export the GraphML
            QString graphml = _exportGraphMLDocument(IDs, false, true);
            if(addAction){
                triggerAction("Replicating Selection");
            }
            //Import the GraphML
            if(node->getParentNode()){

                success = _paste(node->getParentNodeID(),graphml, false);
            }
        }
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
            bool result = _importGraphMLXML(xmlData, getModel());
            if(!result){
                controller_DisplayMessage(CRITICAL, "Import Error", "Cannot import document.", getModel()->getID());
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
                emit controller_ActionProgressChanged(0,"Importing Snippets");
            }
            IMPORTING_SNIPPET = true;
            success = _importGraphMLXML(fileData, parent, false, false);
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

        foreach(int ID, IDs){
            Node* node = getNodeFromID(ID);
            if(node){
                parentNodeKind = node->getParentNode()->getNodeKind();
                break;
            }
        }


        bool readOnly = false;

        //Check if read only.
        if(parentNodeKind == "InterfaceDefinitions"){
            readOnly = askQuestion(MESSAGE, "Export as Read-Only Snippet?", "Would you like to export the current selection as a read-only snippet?");
        }

        QString graphmlRepresentation;


        if(readOnly){
            QString exportTimeStamp = getTimeStamp();

            //Construct the Keys to attach to the nodes to export.
            Key* readOnlyKey = constructKey("readOnly", QVariant::Bool, Entity::EK_NODE);
            Key* IDKey = constructKey("originalID", QVariant::Int, Entity::EK_NODE);
            Key* dateKey = constructKey("exportDateTime", QVariant::String, Entity::EK_NODE);
            Key* annotationKey = constructKey("annotation", QVariant::String, Entity::EK_NODE);

            QList<Node*> nodeList;

            //Construct a list of Nodes to be snippeted
            foreach(int ID, IDs){
                Node* node = getNodeFromID(ID);
                if(node){
                    if(!nodeList.contains(node)){
                        nodeList += node;

                        //Add exported Data.
                        Data* dateData = new Data(dateKey);
                        Data* annotationData = new Data(annotationKey);
                        dateData->setValue(exportTimeStamp);
                        annotationData->setValue("Exported from MEDEA!");
                        node->addData(dateData);
                        node->addData(annotationData);
                    }

                    foreach(Node* child, node->getChildren()){
                        if(!nodeList.contains(child)){
                            nodeList += child;
                        }
                    }
                }
            }

            //Attach read Only Data to the top.

            //Attach read only Data.
            foreach(Node* node, nodeList){
                Data* readOnlyData = new Data(readOnlyKey);
                readOnlyData->setValue(true);
                //Attach data as private data
                node->addData(readOnlyData);

                if(!node->getData(IDKey)){
                    Data* idData = new Data(IDKey);
                    idData->setValue(node->getID());
                    node->addData(idData);

                }
            }
            //Export the GraphML for those Nodes.
            graphmlRepresentation = _exportGraphMLDocument(IDs, false, false);

            //Remove attached Data.
            foreach(Node* node, nodeList){
                Data* readOnlyData = node->getData(readOnlyKey);
                Data* dateData = node->getData(dateKey);
                Data* annotationData = node->getData(annotationKey);

                if(readOnlyData){
                    node->removeData(readOnlyData);
                    delete readOnlyData;
                }
                if(dateData){
                    node->removeData(dateData);
                    delete dateData;
                }
                if(annotationData){
                    node->removeData(annotationData);
                    delete annotationData;
                }
            }
        }else{
            graphmlRepresentation = _exportGraphMLDocument(IDs, false, false);
        }


        snippetData = graphmlRepresentation;
    }
    return snippetData;
}

/**
 * @brief NewController::getAdoptableNodeKinds Gets the list of NodeKinds that the node (From ID) can adopt.
 * @param ID The ID of the parent Node
 * @return A list of all adoptable node kinds.
 */
QStringList NewController::getAdoptableNodeKinds(int ID)
{
    QStringList adoptableNodeKinds;

    Node* parent = getNodeFromID(ID);

    if(parent){
        //Ignore all children for read only kind.
        if(parent->isReadOnly()){
            return adoptableNodeKinds;
        }

        QString parentNodeKind = parent->getDataValue("kind").toString();

        foreach(QString nodeKind, getGUIConstructableNodeKinds()){
            //Construct a Node of the Kind nodeKind.
            bool ignoreKind = false;
            Node* node = constructTypedNode(nodeKind, true);
            if(node){
                //Ignore AggregateInstance for all kinds except Aggregate's
                if(nodeKind == "AggregateInstance" || nodeKind == "VectorInstance"){
                    if(parentNodeKind != "Aggregate" && parentNodeKind != "Variable" && parentNodeKind != "Vector"){
                        ignoreKind = true;
                    }
                }

                if(!ignoreKind && parent->canAdoptChild(node)){
                    if(!adoptableNodeKinds.contains(nodeKind)){
                        adoptableNodeKinds.append(nodeKind);
                    }
                }

                //If we have made a new Node we should delete it.
                if(!protectedNodes.contains(node)){
                    //Clear Memory
                    delete node;
                }
            }
        }
    }
    return adoptableNodeKinds;
}

QList<int> NewController::getFunctionIDList()
{
    QList<int> IDs;
    if(workerDefinitions){
        foreach(Node* node, workerDefinitions->getChildrenOfKind("Process")){
            IDs.append(node->getID());
        }
    }
    return IDs;
}


QList<int> NewController::getConnectableNodes(int srcID)
{
    QList<int> legalNodes;
    Node* src = getNodeFromID(srcID);
    if(src){
        foreach (int ID, nodeIDs) {
            Node* dst = getNodeFromID(ID);
            if(dst && (ID != srcID)){

                if (src->canConnect(dst) != Edge::EC_NONE){
                    legalNodes << ID;
                }else if (dst->canConnect(src) != Edge::EC_NONE){
                    legalNodes << ID;
                }

            }
        }
    }
    return legalNodes;
}

/**
 * @brief NewController::getConnectedNodes Gets the list of connected Node IDs to a Node.
 * @param ID
 * @return
 */
QList<int> NewController::getConnectedNodes(int ID)
{
    QList<int> connectedIDs;
    Node* node = getNodeFromID(ID);
    if(node){
        foreach (Edge* edge, node->getEdges(0)) {
            connectedIDs << edge->getDestination()->getID();
            connectedIDs << edge->getSource()->getID();
        }
    }
    //Remove the node from it's list of connected items.
    connectedIDs.removeAll(ID);
    return connectedIDs;
}

QStringList NewController::getValidKeyValues(QString keyName, int nodeID)
{
    Key* key = getKeyFromName(keyName);
    if(key){
        QString nodeKind;
        if(nodeID != -1){
            Node* node = getNodeFromID(nodeID);
            nodeKind = node->getNodeKind();
        }

        if(nodeKind != ""){
            return key->getValidValues(nodeKind);
        }else{
            return key->getValidValues();
        }
    }
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

int NewController::getSharedParent(int ID1, int ID2)
{
    Node* node1 = getNodeFromID(ID1);
    Node* node2 = getNodeFromID(ID2);
    if(node1 && node2){
        QString treeString;
        QList<int> node1Tree = node1->getTreeIndex();
        QList<int> node2Tree = node2->getTreeIndex();


        while(!node1Tree.isEmpty() && !node2Tree.isEmpty()){
            int index1 = node1Tree.takeFirst();
            int index2 = node2Tree.takeFirst();
            if(index1 == index2){
                treeString += QString::number(index1) +",";
            }else{
                break;
            }
        }
        if(treeString.endsWith(",")){
            treeString.chop(1);
        }

        return treeLookup[treeString];
    }
    return -1;
}

/**
 * @brief NewController::getContainedAspect - Gets the ID of the aspect
 * @param ID The ID of the aspect
 * @return
 */
int NewController::getContainedAspect(int ID)
{
    Node* node = getNodeFromID(ID);
    if(node){
        if(behaviourDefinitions->isAncestorOf(node)){
            return behaviourDefinitions->getID();
        }else if(interfaceDefinitions->isAncestorOf(node)){
            return interfaceDefinitions->getID();
        }else if(assemblyDefinitions->isAncestorOf(node)){
            return assemblyDefinitions->getID();
        }else if(hardwareDefinitions->isAncestorOf(node)){
            return hardwareDefinitions->getID();
        }
    }
    return -1;
}

void NewController::projectSaved(bool success, QString filePath)
{
    if(success){
        setProjectDirty(false);
        //Update the file save path.
        setProjectFilePath(filePath);
    }
}

/**
 * @brief NewController::connectViewAndSetupModel Called
 * @param view
 */
void NewController::connectViewAndSetupModel(NodeView *view)
{
    connectView(view);
    initializeModel();
}

QList<int> NewController::getNodesOfKind(QString kind, int ID, int depth)
{
    QList<int> children;
    if(ID == -1){
        ID = getModel()->getID();
    }
    Node* parentNode = getNodeFromID(ID);
    if(parentNode){
        foreach(Node* child, parentNode->getChildrenOfKind(kind, depth)){
            children.append(child->getID());
        }
    }
    return children;
}

QString NewController::getData(int ID, QString key)
{
    Entity* item = getGraphMLFromID(ID);
    if(item){
        Data* data = item->getData(key);
        if(data){
            return data->getValue().toString();
        }
    }
    return "";
}

bool NewController::isInModel(int ID)
{
    Entity* item = getGraphMLFromID(ID);
    if(item){
        return _isInModel(item);
    }
    return false;
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
    Key* newKey = new Key(name, type, entityKind);

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
    }
    if(name == "middleware"){
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Model";
        validValues << "tao" << "rtidds" << "opensplice" << "coredx" << "tcpip" << "qpidpb" ;
        newKey->addValidValues(validValues, keysValues);
    }

    if(type == QVariant::Bool){
        QStringList validValues;
        validValues << "true" << "false";
        newKey->addValidValues(validValues);
    }

    if (name == "actionOn") {
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Process";
        validValues << "Activate" << "Preprocess" << "Mainprocess" << "Postprocess" << "Passivate";
        newKey->addValidValues(validValues, keysValues);
    }
    if(name == "replicate_count"){
        QPair<qreal, qreal> range;
        range.first = 1;
        range.second = 999;
        newKey->addValidRange(range, QStringList("ComponentAssembly"));
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

    connect(newKey, SIGNAL(validateError(QString,QString,int)), this, SLOT(displayMessage(QString,QString,int)));
    //Add it to the list of Keys.
    keys.append(newKey);

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

        keys.removeAll(key);
        delete key;
        return true;
    }
    return false;
}

Key *NewController::getKeyFromName(QString name)
{
    foreach(Key* key, keys){
        if(key->getName() == name){
            return key;
        }
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


Edge *NewController::_constructEdge(Node *source, Node *destination)
{
    if(!source || !destination){
        qCritical() << "Source or Destination Node is Null!";
        return 0;
    }
    Edge::EDGE_CLASS edgeToMake = source->canConnect(destination);
    if(edgeToMake != Edge::EC_NONE){
        QString sourceKind = source->getDataValue("kind").toString();
        QString destinationKind = destination->getDataValue("kind").toString();

        if(sourceKind == "InputParameter" || destinationKind == "ReturnParameter"){
            //Rotate
            Node* temp = source;
            source = destination;
            destination = temp;
        }

        Edge* edge = constructTypedEdge(source, destination, edgeToMake);
        return edge;
    }else{
        if(!source->gotEdgeTo(destination)){
            //qCritical() << "Edge: Source: " << source->toString() << " to Destination: " << destination->toString() << " Cannot be created!";
        }
        return 0;
    }
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
        bool sendSignal = true;
        IDLookupGraphMLHash[ID] = item;
        if(item->getEntityKind() == Entity::EK_NODE){
            QString nodeKind = ((Node*)item)->getNodeKind();
            kindLookup[nodeKind].append(ID);
            reverseKindLookup[ID] = nodeKind;
            QString treeIndexStr = ((Node*)item)->getTreeIndexString();

            treeLookup[treeIndexStr] = ID;
            reverseTreeLookup[ID] = treeIndexStr;

            nodeIDs.append(ID);

            //Check if we should tell the views
            sendSignal = _isInModel(item);
        }else if(item->getEntityKind() == Entity::EK_EDGE){
            edgeIDs.append(ID);
        }

        EntityAdapter* entityAdapter = 0;

        if(item->isNode()){
            entityAdapter = new NodeAdapter((Node*)item);
        }else if(item->isEdge()){
            entityAdapter = new EdgeAdapter((Edge*)item);
        }

        if(entityAdapter){
            //Store it
            ID2AdapterHash[ID] = entityAdapter;
            emit controller_EntityConstructed(entityAdapter);
        }

        if(viewSignalsEnabled && sendSignal){
            emit controller_GraphMLConstructed(item);
        }else{
            ViewSignal signal;
            signal.id = ID;
            signal.constructSignal = true;
            signal.kind = item->getGraphMLKind();
            viewSignalsList.append(signal);
        }
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
        EntityAdapter* entityAdapter = ID2AdapterHash[ID];

        if(entityAdapter){
            entityAdapter->invalidate();
            emit controller_EntityDestructed(entityAdapter);

            ID2AdapterHash.remove(ID);
        }



        if(item)
        {

            if(item->isNode()){
                Node* node = (Node*)item;
                QString nodeLabel = node->getDataValue("label").toString();
                QString nodeKind = node->getDataValue("kind").toString();


                if(nodeKind == "HardwareNode"){
                    hardwareNodes.remove(nodeLabel);
                }else if(nodeKind == "HardwareCluster"){
                    hardwareClusters.remove(nodeLabel);
                }else if(nodeKind == "ManagementComponent"){
                    managementComponents.remove(nodeLabel);
                }else if(nodeKind == "Process"){
                    if(_isInWorkerDefinitions(node)){
                    //If we are removing a Process contained in the WorkerDefinitions section.
                        QString processName = getProcessName((Process*)node);
                        if(processName != ""){
                            workerProcesses.remove(processName);
                        }
                    }
                }
            }



            if(viewSignalsEnabled){
                emit controller_GraphMLDestructed(ID, item->getGraphMLKind());
            }else{
                ViewSignal signal;
                signal.id = ID;
                signal.constructSignal = false;
                signal.kind = item->getGraphMLKind();
                viewSignalsList.append(signal);
            }
        }

        //qCritical() << "Removing Item ID: " << ID;

        if(reverseKindLookup.contains(ID)){
            QString kind = reverseKindLookup[ID];
            if(kind != ""){
                kindLookup[kind].removeAll(ID);
                reverseKindLookup.remove(ID);
            }
        }

        if(reverseTreeLookup.contains(ID)){
            QString treeStr = reverseTreeLookup[ID];
            reverseTreeLookup.remove(ID);
            treeLookup.remove(treeStr);
        }

        if(reverseReadOnlyLookup.contains(ID)){
            int originalID = reverseReadOnlyLookup[ID];
            reverseReadOnlyLookup.remove(ID);
            readOnlyLookup.remove(originalID);
        }

        IDLookupGraphMLHash.remove(ID);

        if(item->getEntityKind() == Entity::EK_NODE){
            nodeIDs.removeOne(ID);
        }else if(item->getEntityKind() == Entity::EK_EDGE){
            edgeIDs.removeOne(ID);
        }
        if(IDLookupGraphMLHash.size() != (nodeIDs.size() + edgeIDs.size())){
            qCritical() << "Hash Map Inconsistency detected!";
        }


    }
}

Node *NewController::constructChildNode(Node *parentNode, QList<Data *> nodeData)
{
    //Construct the node with the data.
    Node* node = constructNode(nodeData);

    if(!node){
        qCritical() << "Node was not successfully constructed!";
        return 0;
    }

    bool inModel = _isInModel(node);

    //If we have no parentNode, attempt to attach it to the Model.
    if(!parentNode){
        parentNode = model;
    }

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

            constructNodeGUI(node);
        }else{
            destructNode(node);
            return 0;
        }
    }

    if(isUserAction()){
        //If the node is a definition, construct an instance/Implementation in each Instance/Implementation of the parentNode.
        foreach(Node* dependant, parentNode->getDependants()){
            constructDependantRelative(dependant, node);
        }
    }

    return node;
}

Node *NewController::constructNode(QList<Data *> nodeData)
{
    //Get the Kind from the data.
    QString childNodeKind = getDataValueFromKeyName(nodeData, "kind");
    QString childNodeType = getDataValueFromKeyName(nodeData, "type");
    QString childNodeLabel = getDataValueFromKeyName(nodeData, "label");


    Node* node = constructTypedNode(childNodeKind, false, childNodeType, childNodeLabel);

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


QList<Data *> NewController::constructDataVector(QString nodeKind, QPointF relativePosition)
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
    if(nodeKind.endsWith("Parameter")){
        labelString = "";
    }
    data.append(new Data(kindKey, nodeKind));
    data.append(new Data(widthKey, -1));
    data.append(new Data(heightKey, -1));


    bool protectLabel = nodeKind.endsWith("Parameter");

    if(!nodeKind.endsWith("Definitions")){
        Data* labelData = new Data(labelKey);
        labelData->setValue(labelString);
        labelData->setProtected(protectLabel);
        data.append(labelData);
    }

    data.append(new Data(sortKey, -1));


    //Attach Node Specific Data.


    if(nodeKind != "Model" && !nodeKind.endsWith("Definitions")){
        data.append(new Data(expandedKey, false));
    }

    if(nodeKind == "ManagementComponent"){
        data.append(new Data(typeKey));

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
        freqData->setValue(1.0);
    }
    if(nodeKind == "Process"){
        Key* codeKey = constructKey("code", QVariant::String,Entity::EK_NODE);
        Key* actionOnKey = constructKey("actionOn",QVariant::String,Entity::EK_NODE);
        Key* workerKey = constructKey("worker",QVariant::String,Entity::EK_NODE);
        Key* folderKey = constructKey("folder", QVariant::String,Entity::EK_NODE);
        Key* fileKey = constructKey("file", QVariant::String,Entity::EK_NODE);
        Key* operationKey = constructKey("operation", QVariant::String,Entity::EK_NODE);
        Key* complexityKey = constructKey("complexity", QVariant::String,Entity::EK_NODE);
        Key* complexityParamsKey = constructKey("complexityParameters", QVariant::String,Entity::EK_NODE);
        Key* parametersKey = constructKey("parameters", QVariant::String,Entity::EK_NODE);
        data.append(new Data(codeKey));
        Data* actionOnData = new Data(actionOnKey);
        actionOnData->setValue("Mainprocess");
        data.append(actionOnData);
        data.append(new Data(workerKey));
        data.append(new Data(complexityParamsKey));

        data.append(new Data(folderKey));
        data.append(new Data(fileKey));
        data.append(new Data(operationKey));
        data.append(new Data(complexityKey));
        data.append(new Data(parametersKey));
    }
    if(nodeKind == "Condition"){
        Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
        data.append(new Data(valueKey));
    }
    if(nodeKind == "Member"){
        Key* keyKey = constructKey("key", QVariant::Bool,Entity::EK_NODE);
        Data* keyData = new Data(keyKey);
        keyData->setValue(false);
        Data* typeData = new Data(typeKey);
        typeData->setValue("String");
        data.append(keyData);
        data.append(typeData);
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
        data.append(new Data(typeKey, QVariant::String));
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
        data.append(new Data(typeKey, "String"));
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
        data.append(new Data(typeKey));
    }
    if(nodeKind == "InEventPort"){
        Key* asyncKey = constructKey("async", QVariant::Bool,Entity::EK_NODE);
        data.append(new Data(asyncKey, true));
    }
    if(nodeKind.endsWith("Parameter")){
        if(nodeKind == "InputParameter"){
            Key* valueKey = constructKey("value", QVariant::String,Entity::EK_NODE);
            data.append(new Data(valueKey));
        }
        data.append(new Data(typeKey));
    }

    return data;
}

QList<Data *> NewController::constructPositionDataVector(QPointF point)
{
    Key* xKey = constructKey("x", QVariant::Double,Entity::EK_NODE);
    Key* yKey = constructKey("y", QVariant::Double,Entity::EK_NODE);

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
        QList<int> duplicateNumbers;

        //If we have no parent node we don't need to enforce unique labels.
        foreach(Node* sibling, node->getSiblings()){
            QString siblingLabel = sibling->getDataValue("label").toString();


            QRegularExpression regex(newLabel + "(_)?([0-9]+)?");
            QRegularExpressionMatch match = regex.match(siblingLabel);
            if(match.hasMatch()){
                gotMatches = true;
                QString underscore = match.captured(1);
                QString numberStr = match.captured(2);
                if(underscore != "_"){
                    duplicateNumbers += 0;
                }else{
                    bool isInt = false;
                    int number = numberStr.toInt(&isInt);
                    if(isInt){
                        duplicateNumbers += number;
                    }
                }
            }
        }
        qSort(duplicateNumbers);

        int newNumber = -1;
        if(gotMatches){
            newNumber = duplicateNumbers.size();
            for(int i = 0; i < duplicateNumbers.size(); i ++){
                int nextNumber = duplicateNumbers[i];
                if(nextNumber != i){
                    newNumber = i;
                    break;
                }
            }
        }


        if(newNumber > 0){
            QString questionLabel = newLabel + "_" + QString::number(newNumber);
            emit controller_DisplayMessage(WARNING, "Label isn't unique", "Found sibling entity with Label: '" + newLabel + "'. Setting '" + questionLabel + "' instead.",node->getID());
            newLabel = questionLabel;
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

void NewController::enforceUniqueSortOrder(Node *node, int newSortPos)
{
    if(!node){
        return;
    }

    Node* parentNode = node->getParentNode();

    if(parentNode){
        int maxSortPos = parentNode->childrenCount() - 1;

        int currentSortPos = node->getDataValue("sortOrder").toInt();


        if(currentSortPos == -1){
            //If the currentSortPos is invalid, set it as the maximum.
            currentSortPos = maxSortPos;
        }

        if(newSortPos == -1){
            //If the new position is -1, set it to it's current value.
            newSortPos = currentSortPos;
        }

        //Bound the new Sort position.
        if(newSortPos > maxSortPos){
            newSortPos = maxSortPos;
        }else if(newSortPos < 0){
            newSortPos = 0;
        }

        int lowerPos = qMin(currentSortPos, newSortPos);
        int upperPos = qMax(currentSortPos, newSortPos);

        //If we are updating. refactor.
        if(currentSortPos == newSortPos){
            lowerPos = currentSortPos;
            upperPos = maxSortPos;
        }

        int modifier = 1;
        if(newSortPos > currentSortPos){
            modifier = -1;
        }


        foreach(Node* sibling, node->getSiblings()){
            int siblingSortPos = sibling->getDataValue("sortOrder").toInt();

            if(siblingSortPos >= lowerPos && siblingSortPos <= upperPos){
                sibling->setDataValue("sortOrder", siblingSortPos + modifier);
            }
        }

        node->setDataValue("sortOrder", newSortPos);
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
    int parentID = node->getParentNodeID();

    if(DESTRUCTING_CONTROLLER){
        //If we are destructing the controller; Don't add an undo state.
        addAction = false;
    }else if(!isInModel(ID)){
        //If the item isn't in the Model; Don't add an undo state.
        addAction = false;
    }

    if(addAction && !canDeleteNode(node)){
        //If the item is reverseable action, check if we can actually remove the node.
        return false;
    }



    if(addAction){
        int maxPosition = -1;
        if(node->getParentNode()){
            maxPosition = node->getParentNode()->childrenCount() - 1;
        }
        enforceUniqueSortOrder(node, maxPosition);
    }

    //Get a list of dependants.
    QList<Node*> dependants = node->getDependants();

    //Remove all Edges.
    while(node->hasEdges()){
        Edge* edge = node->getFirstEdge();
        destructEdge(edge);
    }

    //Remove all nodes which depend on this.
    while(!dependants.isEmpty()){
        Node* dependant = dependants.takeFirst();
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
        action.Entity.kind = node->getEntityKind();
        action.Entity.nodeKind = node->getNodeKind();
        action.Entity.XML = _exportGraphMLDocument(node);

        addActionToStack(action);
    }

    removeGraphMLFromHash(ID);

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
    }

    //Remove it from the hash of GraphML
    removeGraphMLFromHash(ID);

    delete edge;
    return true;
}

bool NewController::destructEntity(int ID)
{
    Entity* entity = getGraphMLFromID(ID);
    if(entity){
        if(entity->isNode()){
            return destructNode((Node*)entity);
        }else if(entity->isEdge()){
            return destructEdge((Edge*)entity);
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
                success = _importGraphMLXML(action.Entity.XML, parentNode, true);

                if(!success){
                    //qCritical() << action.Entity.XML;
                }
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
                setData(entity, action.Data.keyName, action.Data.oldValue);
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
/*
bool NewController::_attachData(Entity *item, QList<QStringList> dataList, bool addAction)
{
    QList<Data*> graphMLDataList;
    //Conver the StringList into Data Objects.

    foreach(QStringList data, dataList){
        if(data.size() != 5){
            qCritical() << "Data Cannot be Parsed.";
            continue;
        }

        QString keyName = data.at(0);
        QString keyType = data.at(1);
        QString keyFor = data.at(2);
        QString dataValue = data.at(3);
        bool isProtected = data.at(4) == "true";

        QVariant::Type type = Key::getTypeFromGraphML(keyType);
        Entity::ENTITY_KIND entityKind = Entity::getEntityKind(keyFor);

        Key* key = constructKey(keyName, type, entityKind);
        if(!key){
            qCritical() << "Cannot Construct Key";
            continue;
        }

        Data* graphMLData = new Data(key);
        graphMLData->setValue(dataValue);
        if(!graphMLData){
            qCritical() << "Cannot Construct Data";
            continue;
        }

        graphMLData->setProtected(isProtected);
        graphMLDataList.append(graphMLData);
    }

    return _attachData(item, graphMLDataList, addAction);
}*/

bool NewController::_attachData(Entity *item, QList<Data *> dataList, bool addAction)
{
    if(!item){
        return false;
    }

    foreach(Data* data, dataList){
        QString keyName = data->getKeyName();
        //Check if the item has a Data already.
        if(item->getData(keyName)){
            setData(item, keyName, data->getValue(), addAction);
        }else{
            attachData(item, data, addAction);
        }

        Data* updateData = item->getData(keyName);
        if(updateData){
            updateData->setProtected(data->isProtected());
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
    //Get Current Action ID and action.
    //action.Action.ID = currentActionID;
    //action.Action.name = currentAction;
    //action.Action.actionID = currentActionItemID++;
    action.Action.timestamp = getTimeStamp();

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

void NewController::undoRedo(bool undo)
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

    //Lock the GUI.

    controller_SetViewEnabled(false);


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
            int percentage = (actionsReversed * 100) / actionCount;
            if(UNDOING){
                controller_ActionProgressChanged(percentage, "Undoing");
            }
            if(REDOING){
                controller_ActionProgressChanged(percentage, "Redoing");
            }
        }
    }
    retryCount.clear();
    controller_ActionProgressChanged(100);


    if(UNDOING){
        undoActionStack = actionStack;
    }else{
        redoActionStack = actionStack;
    }




    controller_SetViewEnabled(true);

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

void NewController::modelLabelChanged()
{
    Model* model = getModel();
    if(model){
        emit controller_ProjectNameChanged(model->getDataValue("label").toString());
    }
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
        if(hardwareNodes.contains(nodeLabel)){
            return hardwareNodes[nodeLabel];
        }else{
            HardwareNode* hN = new HardwareNode();
            if(storeNode && nodeLabel != ""){
                hardwareNodes[nodeLabel] = hN;
            }
            return hN;
        }
    }else if(nodeKind == "HardwareCluster"){
        if(hardwareClusters.contains(nodeLabel)){
            return hardwareClusters[nodeLabel];
        }else{
            HardwareCluster* hC = new HardwareCluster();
            if(storeNode && nodeLabel != ""){
                hardwareClusters[nodeLabel] = hC;
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
    }else if(nodeKind == "Process"){
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
    }else{
        qCritical() << "Node Kind:" << nodeKind << " not yet implemented!";
        return new BlankNode();
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
        if(parentNode->getNodeKind() == "Variable"){
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
    connect(labelData, SIGNAL(dataChanged(int,QString,QVariant)), this, SLOT(modelLabelChanged()));

    //Update the view with the correct Model Label.
    modelLabelChanged();

    //Construct the top level parents.
    interfaceDefinitions = constructChildNode(model, constructDataVector("InterfaceDefinitions"));
    behaviourDefinitions = constructChildNode(model, constructDataVector("BehaviourDefinitions"));
    deploymentDefinitions =  constructChildNode(model, constructDataVector("DeploymentDefinitions"));

    //Construct the second level containers.
    assemblyDefinitions =  constructChildNode(deploymentDefinitions, constructDataVector("AssemblyDefinitions"));
    hardwareDefinitions =  constructChildNode(deploymentDefinitions, constructDataVector("HardwareDefinitions"));

    protectedNodes << model;
    protectedNodes << interfaceDefinitions;
    protectedNodes << behaviourDefinitions;
    protectedNodes << deploymentDefinitions;
    protectedNodes << assemblyDefinitions;
    protectedNodes << hardwareDefinitions;
    protectedNodes << workerDefinitions;


    setupManagementComponents();
    setupLocalNode();
    //Clear the Undo/Redo Stacks
    undoActionStack.clear();
    redoActionStack.clear();

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
                setData(child, "label", def_Label->getValue().toString() + "_Impl");
            }else{
                setData(child, "label", def_Label->getValue().toString() + "_Inst");
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

    if(inputTopParent){
        //If we are connecting to an Variable, we don't want to bind.
        QString parentNodeKind = inputTopParent->getNodeKind();
        if(parentNodeKind == "Variable"){
            return true;
        }
    }

    Data* definitionData = output->getData("type");
    Data* valueData = input->getData("value");

    if(outputTopParent){
        //Bind Parent Label if we are a variable.
        QString parentNodeKind = outputTopParent->getNodeKind();
        if(parentNodeKind == "Variable"){
            definitionData = outputTopParent->getData("label");
        }
    }

    if(definitionData && valueData){
        if(setup){
            valueData->setParentData(definitionData);
        }else{
            valueData->unsetParentData();
            valueData->clearValue();
        }
    }else{
        return false;
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
                        ReturnParameter* returnParameter = dynamic_cast<ReturnParameter*>(child);
                        if(returnParameter){
                            Data* returnType = returnParameter->getData("type");
                            if(setup){
                                returnType->setParentData(vectorType);
                            }else{
                                returnType->unsetParentData();
                                returnType->clearValue();
                            }
                        }
                    }
                }
            }

        }
    }


    return true;
}

bool NewController::teardownDataEdgeRelationship(BehaviourNode *output, BehaviourNode *input)
{
    Data* typeData = output->getData("type");
    Data* valueData = input->getData("value");

    if(typeData && valueData){
        valueData->unsetParentData();
    }else{
        return false;
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
        Data* value = parameter->getData("value");

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
    }

    storeGraphMLInHash(edge);
}


void NewController::setupManagementComponents()
{
    //EXECUTION MANAGER
    QList<Data*> executionManagerData = constructDataVector("ManagementComponent") ;
    QList<Data*> dancePlanLauncherData = constructDataVector("ManagementComponent") ;
    QList<Data*> ddsLoggingServerData = constructDataVector("ManagementComponent") ;
    QList<Data*> qpidBrokerData = constructDataVector("ManagementComponent") ;

    foreach(Data* data, executionManagerData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("DANCE_EXECUTION_MANAGER");
            data->setProtected(true);
        }
    }
    foreach(Data* data, dancePlanLauncherData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("DANCE_PLAN_LAUNCHER");
            data->setProtected(true);
        }
    }

    foreach(Data* data, ddsLoggingServerData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("DDS_LOGGING_SERVER");
            data->setProtected(true);
        }
    }

    foreach(Data* data, qpidBrokerData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("QPID_BROKER");
            data->setProtected(true);
        }
    }

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
    Data* data = new Data(localhostKey);
    data->setProtected(true);
    data->setValue(true);
    localNodeData.append(data);


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
    default:
        returnable = new Edge(src, dst);
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
    QString data;
    if(model){
        data = _exportGraphMLDocument(model);
    }
    return data;
}

QString NewController::getSelectionAsGraphMLSnippet(QList<int> IDs)
{
    QString data;
    if(model){
        data = _exportSnippet(IDs);
    }
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
            emit controller_DisplayMessage(WARNING, "Cannot open Log File to write", "MEDEA Cannot open log file: " + filePath + ". Logging Disabled");
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
    emit controller_DisplayMessage(MODEL, title, message, ID);
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
        setData(graphML, keyName, dataValue, true);
    }
}


void NewController::constructDestructMultipleEdges(QList<int> srcIDs, int dstID)
{

    QList<Edge*> edgesToDelete;
    bool allAlreadyDeployed = true;

    Node* dst = getNodeFromID(dstID);
    //Look for destructs
    foreach(int ID, srcIDs){
        int deployedID = getDeployedHardwareID(ID);

        if(dstID != deployedID){
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

    triggerAction("Deploying Multiple Entities to singular Hardware Entity.");

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

        //Construct new edges.
        foreach (int srcID, srcIDs) {
            Node* src = getNodeFromID(srcID);

            Edge* edge = constructEdgeWithData(src, dst);
            if(!edge){
                //Try swap
                constructEdgeWithData(dst, src);
            }
            if(!src || !src->gotEdgeTo(dst)){
                emit controller_DisplayMessage(WARNING, "Deployment Failed", "Cannot Connect Entity: " + src->toString() + " to Hardware Entity: " + dst->toString(), srcID);
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
    IMPORTING_PROJECT = true;
    if(!_importProjects(xmlDataList)){
        emit controller_ActionProgressChanged(100);
    }
    IMPORTING_PROJECT = false;
    emit controller_ActionFinished();
}


/**
 * @brief NewController::importSnippet Imports a Snippet of GraphML into the selection defined by ID provided
 * @param IDs - The list of entity IDs
 * @param fileName - The name of the Snippet imported LABEL.<PARENT_KIND>.snippet
 * @param fileData - The graphml data of the snippet.
 */
void NewController::importSnippet(QList<int> IDs, QString fileName, QString fileData)
{
    if(!_importSnippet(IDs, fileName, fileData)){
        emit controller_ActionProgressChanged(100);
    }
    emit controller_ActionFinished();
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

bool NewController::_importGraphMLXML(QString document, Node *parent, bool linkID, bool resetPos)
{
    //Key Lookup provides a way for the original key "id" to be linked with the internal object Key
    QMap<QString , Key*> keyLookup;

    //Node lookup provides a way for the original edge source/target ID's to be linked with the internal object Node
    QMap<QString, Node *> nodeLookup;

    //A list for storing the current Parse <data> tags owned by a <node>
    QList<Data*> currentNodeData;

    //A list storing all the Edge information (source, target, data)
    QList<EdgeTemp> currentEdges;
    Key * currentKey;

    //Used to keep track of state inside the XML
    Entity::ENTITY_KIND nowParsing = Entity::EK_NONE;

    GraphML::GRAPHML_KIND nowInside = GraphML::GK_NONE;
    Entity::ENTITY_KIND nowInsideEntity = Entity::EK_NONE;

    //Used to store the ID of the node we are to construct
    QString nodeID;

    //Used to store the ID of the graph we are to construct
    QString graphID;

    //Used to store the ID of the new node we will construct later.
    QString newNodeID;

    QList<int> readOnlyIDs;
    QHash<int, QStringList> readOnlyIDHashNeeded;



    //If we have been passed no parent, set it as the graph of this Model.
    if(!parent){
        parent = getModel();
    }


    if(parent->isInstance() || parent->isImpl()){
        if(!(UNDOING || REDOING)){
            emit controller_DisplayMessage(WARNING, "Error", "Cannot import or paste into an Instance/Implementation.", parent->getID());
            return false;
        }
    }

    if(!isGraphMLValid(document)){
        emit controller_DisplayMessage(CRITICAL, "Import Error", "Cannot import; invalid GraphML document.", parent->getID());
        return false;
    }



    Node* originalParent = parent;


    bool readOnlyTag = false;
    int originalID = -1;
    int currentROID = -1;


    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);

    //Get the number of lines in the input GraphML XML String.
    float lineCount = document.count("\n");

    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;

    bool ignoreReadOnly = false;
    int resetIgnoreParentID = 0;


    int previousPercentage = -1;
    //While the document has more lines.
    while(!xml.atEnd()){
        QTime newTimer;
        newTimer.start();
        //Read the next tag
        xml.readNext();


        //Calculate the current percentage
        float lineNumber = xml.lineNumber();
        int currentPercentage = (lineNumber * 100.0 / lineCount);
        if(currentPercentage > previousPercentage){
            previousPercentage = currentPercentage;
            if(!(UNDOING || REDOING || INITIALIZING)){
                controller_ActionProgressChanged(currentPercentage);
            }
        }

        //Get the tagName
        QString tagName = xml.name().toString();

        if(tagName == "edge"){
            //Construct an Intermediate struct containing the information about the Edge
            if(xml.isStartElement()){
                //Parse the Edge element into a EdgeStruct object
                EdgeTemp newEdge;
                newEdge.id = getXMLAttribute(xml, "id");
                newEdge.lineNumber = lineNumber;
                newEdge.source = getXMLAttribute(xml, "source");
                newEdge.target = getXMLAttribute(xml, "target");

                //Append to the list of Edges found.
                currentEdges.append(newEdge);

                //Set the current object type to EDGE.
                nowInside = GraphML::GK_ENTITY;
                nowInsideEntity = Entity::EK_EDGE;
            }
            if(xml.isEndElement()){
                //Set the current object type to NONE.
                nowInside = GraphML::GK_NONE;
                nowInsideEntity = Entity::EK_NONE;
            }
        }else if(tagName == "data"){
            if(xml.isStartElement()){
                //Get the datas corresponding Key ID
                QString keyID = getXMLAttribute(xml, "key");

                Key* dataKey = keyLookup[keyID];

                if(!dataKey){
                    bool isInt = false;
                    int keyIntID = keyID.toInt(&isInt);
                    if(isInt){
                        dataKey = getKeyFromID(keyIntID);
                    }
                }

                if(!dataKey){
                    qCritical() << QString("Line #%1: Cannot find the <key> to match the <data key=\"%2\">").arg(QString::number(xml.lineNumber()), keyID);
                    continue;
                }

                //Get the value of the value of the data tag.
                QString dataValue = xml.readElementText();

                //Construct a Data object out of the xml, using the key found in keyLookup
                Data *data = new Data(dataKey);            
                data->setValue(dataValue);



                if(dataKey->getName() == "readOnly"){
                    readOnlyTag = true;
                    originalID = -1;
                }

                if(!linkID && dataKey->getName() == "sortOrder"){
                    delete data;
                    continue;
                }

                if(dataKey->getName() == "originalID"){
                    //Cast as int
                    bool okay = false;

                    int value = dataValue.toInt(&okay);
                    if(okay){
                        originalID = value;
                    }
                }


                if(resetPos && originalParent == parent){
                    QString keyName = dataKey->getName();
                    if(keyName == "x" || keyName == "y"){
                        data->setValue(-1);
                    }
                }

                //Attach the data to the current object.
                switch(nowInsideEntity){
                //Attach the Data to the TempEdge if we are currently inside an Edge.
                case Entity::EK_EDGE:{
                    currentEdges.last().data.append(data);
                    break;
                }
                    //Attach the Data to the list of Data to be attached to the node.
                case Entity::EK_NODE:{
                    currentNodeData.append(data);
                    break;
                }
                default:
                    //Delete the newly constructed object. We don't need it
                    delete data;
                }
            }
        }else if(tagName == "key"){
            if(xml.isStartElement()){
                nowInside = GraphML::GK_KEY;
                //Parse the Attribute Definition.
                QString name = getXMLAttribute(xml, "attr.name");
                QString typeStr = getXMLAttribute(xml, "attr.type");
                QString forStr = getXMLAttribute(xml, "for");

                QVariant::Type type = Key::getTypeFromGraphML(typeStr);
                Entity::ENTITY_KIND entityKind = Entity::getEntityKind(forStr);

                currentKey = constructKey(name, type, entityKind);

                //Get the Key ID.
                QString keyID = getXMLAttribute(xml,"id");

                //Place the key in the lookup Map.
                keyLookup.insert(keyID, currentKey);
            }
            if(xml.isEndElement()){
                nowInside = GraphML::GK_NONE;
            }
        }else if(tagName =="default"){
            if(nowInside == GraphML::GK_KEY){
                QString defaultValue = xml.readElementText();
                currentKey->setDefaultValue(defaultValue);
            }
        }else if(tagName == "node"){
            //If we have found a new <node> it means we should build the previous <node id=nodeID> node.
            if(xml.isStartElement()){
                //Get the ID of the Node
                newNodeID = getXMLAttribute(xml, "id");
                nowInside = GraphML::GK_ENTITY;
                nowInsideEntity = Entity::EK_NODE;
                nowParsing = Entity::EK_NODE;
            }
            if(xml.isEndElement()){
                //Increase the depth, as we have seen another </node>
                parentDepth++;
                //We have reached the end of a Node, therefore not inside a Node anymore.
                nowInside = GraphML::GK_NONE;
            }
        }else if(tagName == "graph"){
            if(xml.isStartElement()){
                //Get the ID of the Graph, we want to point this at the current Parent.
                graphID = getXMLAttribute(xml, "id");
            }
        }else{
            if(xml.isEndDocument()){
                //Construct the final Node
                nowParsing = Entity::EK_NODE;
            }else{
                nowParsing = Entity::EK_NONE;
            }
        }


        if(nowParsing == Entity::EK_NODE){
            //If we have a nodeID to build
            if(nodeID != ""){
                Node* node = 0;

                bool storeMD5 = false;

                //If we have a read only tag, we should look for the originalID provided.
                //To see if we can find the original Node.
                if(readOnlyTag){
                    if(readOnlyLookup.contains(originalID)){
                        //Get the ID of the version we have of the originalID
                        int nodeID = readOnlyLookup[originalID];
                        node = getNodeFromID(nodeID);


                        //Compare timestamp
                        QString newTimeStamp  = getDataValueFromKeyName(currentNodeData, "exportDateTime");
                        QString currentTimeStamp = node->getDataValue("exportDateTime").toString();


                        //If the date is older.
                        if(newTimeStamp < currentTimeStamp){
                            resetIgnoreParentID = node->getParentNodeID();
                            ignoreReadOnly = !askQuestion(CRITICAL, "Import Older Snippet", "You are trying to replace an newer version of a snippet with an older version. Would you like to proceed?", node->getID());
                        }


                        storeMD5 = !ignoreReadOnly;

                        //Got match, so we don't need to construct item.
                        while(!currentNodeData.isEmpty()){
                            Data* data = currentNodeData.takeFirst();
                            if(!ignoreReadOnly){
                                _attachData(node, data);
                            }
                            if(!data->getParent())
                            {
                                delete data;
                            }
                        }
                    }
                }

                if(!node){
                    Node* newNode = 0;

                    //Don't use
                    if(!OPENING_PROJECT){
                        QString nodeKind = getDataValueFromKeyName(currentNodeData, "kind");
                        if(nodeKind == "Model"){
                            while(!currentNodeData.isEmpty()){
                                delete currentNodeData.takeFirst();
                            }
                            newNode = getModel();
                        }
                    }

                    if(!newNode){
                        //Construct the specialised Node
                        newNode = constructChildNode(parent, currentNodeData);

                        if(!newNode){
                            qCritical() << "Parent: " << parent->toString() << " Cannot Adopt: " << getDataValueFromKeyName(currentNodeData, "kind");
                            //qCritical() << parent;
                            //emit controller_DialogMessage(CRITICAL, "Import Error", QString("Line #%1: entity cannot adopt child entity!").arg(xml.lineNumber()), parent);
                            qDebug() << QString("Line #%1: entity cannot adopt child entity!").arg(xml.lineNumber());
                            emit controller_DisplayMessage(WARNING, "Paste Error", "Cannot import/paste into this entity.", parent->getID());
                        }
                    }
                    node = newNode;
                    storeMD5 = true;
                    currentNodeData.clear();
                }

                if(node){
                    //Set the currentParent to the Node Construced
                    parent = node;
                }else{
                    parentDepth = 0;
                }

                //Navigate back to the correct parent.
                while(parent && parentDepth > 0){
                    parent = parent->getParentNode();
                    parentDepth --;
                }

                if(ignoreReadOnly){
                    if(parent->getID() == resetIgnoreParentID){
                        ignoreReadOnly = false;
                        resetIgnoreParentID = -1;
                    }
                }

                if(node){
                    //Add the new Node to the lookup table.
                    nodeLookup[nodeID] = node;

                    if(readOnlyTag && originalID != -1){
                        readOnlyLookup[originalID] = node->getID();
                        reverseReadOnlyLookup[node->getID()] = originalID;
                        originalID = -1;

                        if(storeMD5){
                            //If the parent of this item isn't read only, we need to add this item to the list of readOnlyIDs
                            if(!node->getParentNode()->isReadOnly()){
                                currentROID = node->getID();
                                readOnlyIDs.append(currentROID);
                            }

                            readOnlyIDHashNeeded[currentROID].append(node->toMD5Hash());
                        }

                        //Locking Data
                        foreach(Data* data, node->getData()){
                            data->setProtected(true);
                        }
                        readOnlyTag = false;
                    }

                    if(linkID){
                        bool okay;
                        int oldID = nodeID.toInt(&okay);
                        if(okay){
                            linkOldIDToID(oldID, node->getID());
                        }
                    }

                    //If we have encountered a Graph object, we should point it to it's parent Node to allow links to Graph's
                    if(graphID != ""){
                        nodeLookup.insert(graphID, node);
                        graphID = "";
                    }
                }
            }
            //Set the current nodeID to equal the newly found NodeID.
            nodeID = newNodeID;
        }
    }


    //Foreach read only items we have imported.
    foreach(int ID, readOnlyIDs){
        Node* node = getNodeFromID(ID);
        QList<Node*> allChildren = node->getChildren();

        QStringList neededHashs = readOnlyIDHashNeeded[ID];


        QList<int> toDeleteIDs;
        //Foreach child in node, check if we are meant to have this item!
        while(!allChildren.isEmpty()){
            Node* child = allChildren.takeLast();
            QString childMD5 = child->toMD5Hash();
            if(neededHashs.contains(childMD5)){
                neededHashs.removeOne(childMD5);
            }else{
                toDeleteIDs.append(child->getID());
            }
        }

        //Remove the items we don't need anymore
        _remove(toDeleteIDs, false);
    }


    if(!(UNDOING || REDOING || INITIALIZING)){
       controller_ActionProgressChanged(0, "Constructing Edges.");
    }

    //Sort the edges into types.
    QList<EdgeTemp> aggregateEdges;
    QList<EdgeTemp> instanceEdges;
    QList<EdgeTemp> implEdges;
    QList<EdgeTemp> otherEdges;
    bool poppedup = false;
    foreach(EdgeTemp edge, currentEdges){
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];

        if(!s){
            s = getNodeFromID(getIntFromQString(edge.source));
        }
        if(!d){
            d = getNodeFromID(getIntFromQString(edge.target));
        }

        if(!(s && d)){
            if(!poppedup){
                emit controller_DisplayMessage(CRITICAL, "Import Error", "Cannot find all end points of imported edge!");
                poppedup = true;
            }
            continue;
        }

        if(d->isDefinition()){
            if(s->isInstance()){
                instanceEdges.append(edge);
            }else if(s->isImpl()){
                implEdges.append(edge);
            }else if(d->getDataValue("kind") == "Aggregate"){
                aggregateEdges.append(edge);
            }else{
                otherEdges.append(edge);
            }
        }else{
            otherEdges.append(edge);
        }
    }

    //Implementations, Instances, Aggregates, then others.
    QList<EdgeTemp> sortedEdges;
    sortedEdges << implEdges;
    sortedEdges << instanceEdges;
    sortedEdges << aggregateEdges;
    sortedEdges << otherEdges;

    //Construct the Edges from the EdgeTemp objects
    int maxRetry = 3;
    QHash<QString, int> retryCount;
    bool errorMessage = false;

    int totalEdges = sortedEdges.size();
    int edgesMade = 1;


    while(sortedEdges.size() > 0){


        EdgeTemp edge = sortedEdges.first();

        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];

        if(!s){
            s = getNodeFromID(getIntFromQString(edge.source));
        }
        if(!d){
            d = getNodeFromID(getIntFromQString(edge.target));
        }


        bool retry = true;
        if(retryCount[edge.id] > maxRetry){
            if(!s->gotEdgeTo(d) && !errorMessage){
                qCritical() << s << d;
                emit controller_DisplayMessage(CRITICAL, "Import Error", "Cannot construct edge!");
                errorMessage = true;
            }
            retry = false;
        }




        Edge* newEdge = constructEdgeWithData(s, d, edge.data, getIntFromQString(edge.id));
        if(newEdge){
            sortedEdges.removeFirst();
            edgesMade ++;
        }else{
            sortedEdges.removeFirst();
            if(!s->gotEdgeTo(d) && retry){
                retryCount[edge.id] += 1;
                sortedEdges.append(edge);
            }else{
                edgesMade ++;
            }
        }

        if(!(UNDOING || REDOING || INITIALIZING)){
              emit controller_ActionProgressChanged((100 * totalEdges)/edgesMade);
          }
    }





    if(!(UNDOING || REDOING || INITIALIZING)){
        controller_ActionProgressChanged(100);
    }

    return true;
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
        emit controller_ProjectRequiresSave(projectDirty);
    }
}

void NewController::setProjectFilePath(QString filePath)
{
    projectFileSavePath = filePath;
    emit controller_ProjectFileChanged(filePath);
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
    return true;
}

bool NewController::canCut(QList<int> selection)
{
    return canCopy(selection) && canDelete(selection);
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
            if(graphml->isReadOnly()){
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
        if(node->getData("readOnly")){
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
    return false;
    /*
    bool gotAnyNonReadOnly=false;
    foreach(int ID, IDs){
        Entity* entity = getGraphMLFromID(ID);
        if(entity && !entity->isReadOnly()){
            gotAnyNonReadOnly = true;
            break;
        }
    }
    return gotAnyNonReadOnly;
    */
}

bool NewController::canUnsetReadOnly(QList<int> IDs)
{
    return false;
    /*
    bool gotAnyReadOnly=false;
    foreach(int ID, IDs){
        Entity* entity = getGraphMLFromID(ID);
        if(entity && entity->isReadOnly()){
            gotAnyReadOnly = true;
            break;
        }
    }
    return gotAnyReadOnly;
    */
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

QString NewController::getProjectFileName()
{
    return projectFileSavePath;
}

bool NewController::projectRequiresSaving()
{
    return projectDirty;
}

bool NewController::isNodeAncestor(int ID, int ID2)
{
    Node* node = getNodeFromID(ID);
    Entity* entity = getGraphMLFromID(ID2);

    if(node && entity){
        return node->isAncestorOf(entity);
    }
    return false;
}

bool NewController::areIDsInSameBranch(int mainID, int newID)
{
    Entity* main = getGraphMLFromID(mainID);
    Entity* other = getGraphMLFromID(newID);

    QList<int> mainTree;
    QList<int> secondTree;
    QList<int> thirdTree;
    bool useThirdTree = false;

    if(main->isNode()){
        mainTree = ((Node*)main)->getTreeIndex();
    }

    if(other->isEdge()){
        Edge* edge = (Edge*)other;
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();
        if(src && dst){
            secondTree = src->getTreeIndex();
            thirdTree = dst->getTreeIndex();
        }
    }
    if(other->isNode()){
        secondTree = ((Node*)other)->getTreeIndex();
    }

    for(int i = 0; i < mainTree.size(); i++){
        int one = mainTree.at(i);
        int two = -1;
        int three = -1;
        if(i < secondTree.size()){
            two = secondTree.at(i);
        }
        if(i < thirdTree.size()){
            three = thirdTree.at(i);
        }

        if(two == -1 || (three == -1 && useThirdTree)){
            return true;
        }
        if(one != two || (one != three && useThirdTree)){
            return false;
        }
    }
    return true;
}

int NewController::getDefinition(int ID)
{
    Node* original = getNodeFromID(ID);
    if(original){
        Node* node = original;
        while (node && node->getDefinition()) {
            node = node->getDefinition();
        }
        if (node != original) {
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
        if(data->getKeyName() == keyName){
            return data->getValue().toString();
        }
    }
    return "";
}

void NewController::setDataValueFromKeyName(QList<Data *> dataList, QString keyName, QString value)
{
    foreach(Data* data, dataList){
        if(data->getKeyName() == keyName){
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
    if(UNDOING || REDOING || OPENING_PROJECT || IMPORTING_PROJECT || INITIALIZING || PASTE_USED){
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
