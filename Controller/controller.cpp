#include "controller.h"
#include "../View/nodeview.h"
#include <QDebug>
#include <algorithm>
#include <QDateTime>
#include <QMessageBox>
#include <QEventLoop>

#include <QSysInfo>

#define LABEL_TRUNCATE_LENGTH 64

bool UNDO = true;
bool REDO = false;
bool SETUP_AS_INSTANCE = true;
bool SETUP_AS_IMPL = false;

NewController::NewController()
{
    qRegisterMetaType<MESSAGE_TYPE>("MESSAGE_TYPE");
    qRegisterMetaType<GraphML::KIND>("GraphML::KIND");
    qRegisterMetaType<QList<int>>("QList<int>");



    logFile = 0;

    USE_LOGGING = false;
    UNDOING = false;
    REDOING = false;
    DELETING = false;
    CUT_USED = false;
    IMPORTING_SNIPPET = false;
    model = 0;
    //Construct
    behaviourDefinitions = 0;
    interfaceDefinitions = 0;
    deploymentDefinitions = 0;
    hardwareDefinitions = 0;
    assemblyDefinitions = 0;

    currentActionID = 0;
    currentActionItemID = 0;
    actionCount = 0;
    currentAction = "";


    viewAspects << "Assembly" << "Workload" << "Definitions" << "Hardware";
    protectedKeyNames << "x" << "y" << "kind"; //<< "width" << "height";


    containerNodeKinds << "Model";
    containerNodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    containerNodeKinds << "HardwareDefinitions" << "AssemblyDefinitions" << "ManagementComponent";
    containerNodeKinds << "HardwareCluster";

    definitionNodeKinds << "IDL" << "Component" << "Attribute" << "ComponentAssembly" << "ComponentInstance" << "BlackBox" << "BlackBoxInstance";
    definitionNodeKinds << "Member" << "Aggregate";
    definitionNodeKinds << "InEventPort"  << "OutEventPort";
    definitionNodeKinds << "InEventPortDelegate"  << "OutEventPortDelegate";
    definitionNodeKinds << "AggregateInstance";
    definitionNodeKinds << "ComponentImpl" << "Vector" << "VectorInstance";







    behaviourNodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload" << "OutEventPortImpl";
    behaviourNodeKinds << "WhileLoop" << "VectorOperation" << "InputParameter" << "ReturnParameter";;


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

    //constructableNodeKinds << "InputParameter" << "ReturnParameter";

    constructableNodeKinds.removeDuplicates();

    guiConstructableNodeKinds.append(definitionNodeKinds);
    guiConstructableNodeKinds.append(behaviourNodeKinds);
    guiConstructableNodeKinds.removeDuplicates();
    guiConstructableNodeKinds.sort();

    //Setup the required Parameters.
    setupParameters();
}

void NewController::connectView(NodeView *view)
{
    view->setController(this);

    //Connect SIGNALS to view Slots (ALL VIEWS)
    connect(this, SIGNAL(controller_GraphMLConstructed(GraphML*)), view, SLOT(constructGUIItem(GraphML*)));

    connect(this, SIGNAL(controller_GraphMLDestructed(int, GraphML::KIND)), view, SLOT(destructGUIItem(int,GraphML::KIND)));

    connect(this, SIGNAL(controller_SetViewEnabled(bool)), view, SLOT(setEnabled(bool)));



    if(view->isMainView()){
        connect(view, SIGNAL(view_ConstructFunctionNode(int, QString, QString, QString, QPointF)), this, SLOT(constructFunctionNode(int,QString,QString,QString,QPointF)));
        connect(this, SIGNAL(controller_CanRedo(bool)), view, SLOT(canRedo(bool)));
        connect(this, SIGNAL(controller_CanUndo(bool)), view, SLOT(canUndo(bool)));
        connect(view, SIGNAL(view_constructDestructEdges(QList<int>, int)), this, SLOT(constructDestructMultipleEdges(QList<int>, int)));
        connect(view, SIGNAL(view_EnableDebugLogging(bool, QString)), this, SLOT(enableDebugLogging(bool, QString)));


        connect(view, SIGNAL(view_DestructEdge(int, int)), this, SLOT(destructEdge(int, int)));
        //Pass Through Signals to GUI.
        connect(view, SIGNAL(view_ClearHistoryStates()), this, SLOT(clearHistory()));
        connect(view, SIGNAL(view_Clear()), this, SLOT(clear()));
        connect(this, SIGNAL(controller_ProjectNameChanged(QString)), view, SIGNAL(view_ProjectNameChanged(QString)));
        connect(this, SIGNAL(controller_ExportedProject(QString)), view, SIGNAL(view_ExportedProject(QString)));
        connect(this, SIGNAL(controller_SetClipboardBuffer(QString)), view, SIGNAL(view_SetClipboardBuffer(QString)));


        connect(this, SIGNAL(controller_ActionProgressChanged(int,QString)), view, SIGNAL(view_updateProgressStatus(int,QString)));

        //Signals to the View.
        connect(this, SIGNAL(controller_DisplayMessage(MESSAGE_TYPE, QString, QString, int)), view, SLOT(showMessage(MESSAGE_TYPE,QString,QString,int)));

        // Re-added this for now

        connect(this, SIGNAL(controller_ExportedSnippet(QString,QString)), view, SIGNAL(view_ExportedSnippet(QString,QString)));

        connect(this, SIGNAL(controller_AskQuestion(MESSAGE_TYPE, QString, QString, int)), view, SLOT(showQuestion(MESSAGE_TYPE,QString,QString,int)));
        connect(view, SIGNAL(view_QuestionAnswered(bool)), this, SLOT(gotQuestionAnswer(bool)));
        connect(this, SIGNAL(controller_ModelReady()), view, SIGNAL(view_ModelReady()));
    }

    if(view->isMainView()){
        connect(this, SIGNAL(controller_ActionFinished()), view, SLOT(actionFinished()));
        connect(view, SIGNAL(view_Replicate(QList<int>)), this, SLOT(replicate(QList<int>)));
        //File SLOTS
        connect(view, SIGNAL(view_ExportProject()), this, SLOT(exportProject()));
        connect(view, SIGNAL(view_ImportProjects(QStringList)), this, SLOT(importProjects(QStringList)));


        connect(view, SIGNAL(view_ImportedSnippet(QList<int>,QString,QString)), this, SLOT(importSnippet(QList<int>,QString,QString)));
        connect(view, SIGNAL(view_ExportSnippet(QList<int>)), this, SLOT(exportSnippet(QList<int>)));

        //Edit SLOTS
        connect(view, SIGNAL(view_Undo()), this, SLOT(undo()));
        connect(view, SIGNAL(view_Redo()), this, SLOT(redo()));
        connect(view, SIGNAL(view_Delete(QList<int>)), this, SLOT(remove(QList<int>)));
        connect(view, SIGNAL(view_Copy(QList<int>)), this, SLOT(copy(QList<int>)));
        connect(view, SIGNAL(view_Cut(QList<int>)), this, SLOT(cut(QList<int>)));
        connect(view, SIGNAL(view_Paste(int,QString)), this, SLOT(paste(int,QString)));

        //Node Slots
        connect(view, SIGNAL(view_ConstructEdge(int,int, bool)), this, SLOT(constructEdge(int, int, bool)));
        connect(view, SIGNAL(view_ConstructNode(int,QString,QPointF)), this, SLOT(constructNode(int,QString,QPointF)));


        connect(view, SIGNAL(view_ConstructConnectedNode(int,int,QString,QPointF)), this, SLOT(constructConnectedNode(int,int,QString,QPointF)));

        //Undo SLOTS
        connect(view, SIGNAL(view_TriggerAction(QString)), this, SLOT(triggerAction(QString)));
        connect(view, SIGNAL(view_SetGraphMLData(int, QString, QString)), this, SLOT(setGraphMLData(int, QString, QString)));
        connect(view, SIGNAL(view_SetGraphMLData(int, QString, qreal)), this, SLOT(setGraphMLData(int, QString, qreal)));


    }
}



void NewController::initializeModel()
{
    setupModel();
    clearHistory();
    emit controller_ModelReady();
}

NewController::~NewController()
{
    //Disable Logging
    enableDebugLogging(false);

    DELETING = true;
    destructNode(model, false);


    // while(keys.size() > 0){
    //     GraphMLKey* key = keys.takeFirst();
    //     delete key;
    //}

}

QString NewController::_exportGraphMLDocument(QList<int> nodeIDs, bool allEdges, bool GUI_USED)
{
    bool exportAllEdges = allEdges;

    QString keyXML, edgeXML, nodeXML;
    QList<Node*> containedNodes;
    QList<GraphMLKey*> containedKeys;
    QList<Edge*> containedEdges;

    //Get all Children and Edges.
    foreach(int ID, nodeIDs){
        Node* node = getNodeFromID(ID);
        if(node){
            if(containedNodes.contains(node) == false){
                containedNodes.append(node);
            }

            //Get all keys used by this node.
            foreach(GraphMLKey* key, node->getKeys())
            {
                //Add the <key> tag to the list of Keys contained.
                if(!containedKeys.contains(key)){
                    containedKeys.append(key);
                    keyXML += key->toGraphML(1);
                }
            }

            //Get all Children in this node.
            foreach(Node* child, node->getChildren()){
                Node* childNode = dynamic_cast<Node*>(child);
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
                foreach(GraphMLKey* key, edge->getKeys()){
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

QStringList NewController::getNodeKinds(Node *parent)
{
    QStringList empty;
    if(parent){
        if(parent->isImpl() || (parent->getParentNode() && parent->getParentNode()->isImpl())){
            return behaviourNodeKinds;
        }else if(!parent->isInstance()){
            return definitionNodeKinds;
        }
    }
    return empty;
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



void NewController::setGraphMLData(GraphML *parent, QString keyName, QString dataValue, bool addAction)
{

    if(DELETING){
        //Ignore any calls to set whilst deleting.
        return;
    }

    if(!parent){
        qCritical() << "view_UpdateGraphMLData() Cannot Update GraphMLData for NULL GraphML object.";
        return;
    }



    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = MODIFIED;
    action.actionKind = GraphML::DATA;
    action.keyName = keyName;
    action.isNum = false;




    GraphMLData* data = parent->getData(keyName);
    if(data->getKey()->isNumber() && !data->getKey()->isBoolean()){
        bool okay = false;
        qreal dataValueNumber = dataValue.toDouble(&okay);
        if(okay){
            setGraphMLData(parent, keyName, dataValueNumber, addAction);
            return;
        }
    }

    if(data){
        action.dataValue = data->getValue();

        if(dataValue == action.dataValue){
            //Don't update if we have got the same value in the model.
            return;
        }

        if(parent->isNode()){
            Node* node = (Node*)parent;
            if(keyName == "label"){
                enforceUniqueLabel(node, dataValue);
            }else{
                data->setValue(dataValue);
            }
        }else if(parent->isEdge()){
            data->setValue(dataValue);
        }

    }else{
        qCritical() << "view_UpdateGraphMLData() Doesn't Contain GraphMLData for Key: " << keyName;
        return;
    }
    if(addAction){
        addActionToStack(action, addAction);
    }
}

/**
 * @brief NewController::attachGraphMLData - Attaches a GraphMLData to a Entity
 * @param parent - The Entity to attach the GraphMLData to.
 * @param data - The GraphMLData to attach
 * @param addAction - Add an undo state
 */
void NewController::attachGraphMLData(GraphML *parent, GraphMLData *data, bool addAction)
{
    if(!parent || !data){
        qCritical() << "attachGraphMLData() parent or data is NULL!";
        return;
    }

    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::DATA;
    action.keyName = data->getKeyName();

    //Attach the Data to the parent
    parent->attachData(data);


    //Add an action to the stack.
    addActionToStack(action, addAction);
}

/**
 * @brief NewController::destructGraphMLData - Removes and destroys a GraphMLData attached to an Entity
 * @param parent - The Entity the GraphMLData is attached to.
 * @param keyName - The Name of the Key of the GraphMLData
 * @param addAction - Add an undo state
 */
void NewController::destructGraphMLData(GraphML *parent, QString keyName, bool addAction)
{  
    if(!parent){
        qCritical() << "destructGraphMLData() parent is NULL!";
        return;
    }

    //Get the Data from the GraphML
    GraphMLData* data = parent->getData(keyName);

    if(!data){
        qCritical() << "destructGraphMLData(): " + parent->toString() + " does not contain GraphMLData for key: " + keyName;
        return;
    }

    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = DESTRUCTED;
    action.actionKind = GraphML::DATA;
    action.keyName = keyName;
    action.dataValues.append(data->toStringList());
    action.boundDataIDs.append(data->getBoundIDS());

    if(data->getParentData()){
        action.parentDataID.append(data->getParentData()->getID());
    }


    //Remove the Data to the parent
    parent->removeData(data);
    //data->deleteLater();
    delete data;

    //Add an action to the stack.
    addActionToStack(action, addAction);
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

void NewController::setupParameters()
{
    //BehaviourNode::addParameter("PeriodicEvent", "frequency", "number", true, "1");
    BehaviourNode::addParameter("Process", "VectorOperation", "get", "index", "number",true);
}

void NewController::setGraphMLData(GraphML *parent, QString keyName, qreal dataValue, bool addAction)
{
    if(DELETING){
        //Ignore any calls to set whilst deleting.
        return;
    }

    if(!parent){
        qCritical() << "view_UpdateGraphMLData() Cannot Update GraphMLData for NULL GraphML object.";
        return;
    }

    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = MODIFIED;
    action.actionKind = GraphML::DATA;
    action.keyName = keyName;
    action.isNum = true;


    GraphMLData* data = parent->getData(keyName);

    if(data){
        if(data->gotDoubleValue()){
            action.dataValueNum = data->getDoubleValue();
        }

        if(parent->isNode() && keyName == "sortOrder"){
            enforceUniqueSortOrder((Node*)parent, dataValue);
            qreal newSortOrder = parent->getDataNumberValue("sortOrder");

            if(action.dataValueNum == newSortOrder){
                //Don't add an action for the initial setting!
                addAction = false;
            }
        }else if(parent->isNode()){
            if(keyName == "x" || keyName == "y"){
                if(action.dataValueNum != -1 && dataValue == -1){
                    return;
                }
                data->setValue(dataValue);
            }else{
                data->setValue(dataValue);
            }
        }else if(parent->isEdge()){
            data->setValue(dataValue);
        }
    }else{
        qCritical() << "view_UpdateGraphMLData() Doesn't Contain GraphMLData for Key: " << keyName;
        return;
    }
    if(addAction){
        addActionToStack(action, addAction);
    }

}





void NewController::constructNode(int parentID, QString kind, QPointF centerPoint)
{
    if(kind != ""){
        Node* parentNode = getNodeFromID(parentID);

        if(kind.endsWith("Parameter")){
            BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(parentNode);
            if(behaviourNode){
                QList<ParameterRequirement*> paramaters = behaviourNode->getNeededParameters();
            }
        }

        triggerAction("Constructing Child Node");
        constructChildNode(parentNode, constructGraphMLDataVector(kind, centerPoint));
    }
    emit controller_ActionFinished();
}

void NewController::constructFunctionNode(int parentID, QString nodeKind, QString className, QString functionName, QPointF position)
{
    Node* parentNode = getNodeFromID(parentID);
    QList<GraphMLData*> dataList = constructGraphMLDataVector(nodeKind, position);

    foreach(GraphMLData* data, dataList){
        if(data->getKeyName() == "worker"){
            data->setValue(className);
        }else if(data->getKeyName() == "operation"){
            data->setValue(functionName);
        }
    }

    //Get Parameters!

    QList<ParameterRequirement*> parameters = BehaviourNode::getParameters(nodeKind);

    foreach(ParameterRequirement* parameter, parameters){
        qCritical() << parameter->getName();
    }




    triggerAction("Constructing Child FunctionNode");
    constructChildNode(parentNode, dataList);
    emit controller_ActionFinished();

}

void NewController::constructEdge(int srcID, int dstID, bool reverseOkay)
{
    Node* src = getNodeFromID(srcID);
    Node* dst = getNodeFromID(dstID);
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
        Edge* edge = src->getConnectingEdge(dst);
        if(edge){
            destructEdge(edge, true);
        }
    }
    emit controller_ActionFinished();
}

void NewController::constructConnectedNode(int parentID, int connectedID, QString kind, QPointF relativePos)
{
    Node* parentNode = getNodeFromID(parentID);
    Node* connectedNode = getNodeFromID(connectedID);
    if(parentNode && connectedNode){
        triggerAction("Constructed Connected Node");
        Node* newNode = constructChildNode(parentNode, constructGraphMLDataVector(kind));
        //bool gotEdge = false;
        if(newNode){
            //Update the position
            setGraphMLData(newNode, "x", relativePos.x());
            setGraphMLData(newNode, "y", relativePos.y());

            constructEdgeWithData(newNode, connectedNode);

            //Try the alternate connection.
            if(!newNode->isConnected(newNode)){
                constructEdgeWithData(connectedNode, newNode);
            }
            //gotEdge = newNode->isConnected(newNode);
        }
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
                destructEdge(src->getConnectingEdge(dst), true);
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


Edge* NewController::constructEdgeWithGraphMLData(Node *src, Node *dst, QList<GraphMLData *> data, int previousID)
{
    Edge* edge = _constructEdge(src, dst);
    if(edge){
        _attachGraphMLData(edge, data, false);

        //If we are Undo-ing or Redo-ing and we have an ID to link it to.
        if((UNDOING || REDOING) && previousID != -1){
            linkOldIDToID(previousID, edge->getID());
        }
        constructEdgeGUI(edge);
    }
    return edge;
}

Edge* NewController::constructEdgeWithData(Node *src, Node *dst, QList<QStringList> attachedData, int previousID)
{
    Edge* edge = _constructEdge(src, dst);
    if(edge){
        _attachGraphMLData(edge, attachedData, false);

        if((UNDOING || REDOING) && previousID != -1){
            linkOldIDToID(previousID, edge->getID());
        }

        constructEdgeGUI(edge);
    }


    if(!src->isConnected(dst)){
        qCritical() << "Edge: " << src->toString() << " to " << dst->toString() << " not legal.";
        qCritical() << "Edge not legal";
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
    if(!_remove(IDs)){
        controller_DisplayMessage(WARNING, "Delete Error", "Cannot delete all selected entities.");
        emit controller_ActionProgressChanged(100);
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
            if(addAction){
                triggerAction("Pasting Selection.");
                emit controller_ActionProgressChanged(0, "Pasting Selection");
            }

            //Paste it into the current Selected Node,
            success = _importGraphMLXML(xmlData, parentNode, CUT_USED, true);
            if(!success){

            }
            CUT_USED = false;
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
    if(!canDelete(IDs)){
        return false;
    }
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
            connectedLinkedIDs.clear();
            GraphML* graphML = getGraphMLFromID(ID);


            if(graphML){
                if(graphML->isNode()){
                    Node* node = (Node*)graphML;

                    bool success = destructNode(node, true);
                    if(!success){
                        allSuccess = false;
                    }

                }else if (graphML->isEdge()){
                    //destructEdge((Edge*)graphML);
                    bool success = destructEdge((Edge*)graphML);
                    if(!success){
                        allSuccess = false;
                    }
                }
            }
            //Add any related ID's which need deleting to the top of the stack.
            IDs = connectedLinkedIDs + IDs;
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
                success = _paste(node->getParentNode()->getID(),graphml, false);
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
bool NewController::_exportSnippet(QList<int> IDs)
{
    bool success = false;
    if(canExportSnippet(IDs)){
        CUT_USED = false;

        //Export the GraphML for those Nodes.
        QString graphmlRepresentation = _exportGraphMLDocument(IDs, false, false);

        Node* parent = getSharedParent(IDs);
        QString parentKind = parent->getDataValue("kind");
        if(parent && parentKind != "" && graphmlRepresentation != ""){
            controller_ExportedSnippet(parentKind, graphmlRepresentation);
            success = true;
        }
    }
    return success;
}

/**
 * @brief NewController::_exportProject Exports the Entire GraphML Model.
 * Calls controller_ExportedProject on success.
 * @return Action successful.
 */
bool NewController::_exportProject()
{
    if(model){
        QString data = _exportGraphMLDocument(model);
        controller_ExportedProject(data);
        return true;
    }
    return false;
}

QStringList NewController::getAdoptableNodeKinds(int ID)
{
    QStringList adoptableNodeTypes;

    Node* parent = getNodeFromID(ID);

    if(parent){
        foreach(QString nodeKind, getNodeKinds(parent)){
            //Construct a Node of the Kind nodeKind.

            //Ignore AggregateInstance for all kinds except Aggregate's
            if(nodeKind == "AggregateInstance"){
                if(parent->getDataValue("kind") != "Aggregate"){
                    continue;
                }
            }

            Node* node = constructTypedNode(nodeKind);

            if(!node){
                continue;
            }

            if(parent->canAdoptChild(node)){
                if(!adoptableNodeTypes.contains(nodeKind)){
                    adoptableNodeTypes.append(nodeKind);
                }
            }
        }
    }

    return adoptableNodeTypes;
}

QList<int> NewController::getConnectableNodes(int srcID)
{
    QList<int> legalNodes;
    Node* src = getNodeFromID(srcID);
    if(src){
        foreach (int ID, nodeIDs) {
            Node* dst = getNodeFromID(ID);
            if(dst && ID != srcID){
                if (src->canConnect(dst)){
                    legalNodes << ID;
                }else if (dst->canConnect(src)){
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
    GraphMLKey* key = getGraphMLKeyFromName(keyName);
    if(key){
        QString nodeKind;
        if(nodeID != -1){
            Node* node = getNodeFromID(nodeID);
            nodeKind = node->getNodeKind();
        }

        if(nodeKind != ""){
            return key->getSelectableValues(nodeKind);
        }else{
            return key->getSelectableValues();
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

        while(!node1Tree.isEmpty() || !node2Tree.isEmpty()){
            int index1 = node1Tree.takeFirst();
            int index2 = node2Tree.takeFirst();
            if(index1 == index2){
                treeString += QString::number(index1) +",";
            }else{
                break;
            }
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



GraphMLKey *NewController::constructGraphMLKey(QString name, QString type, QString forString)
{
    //Construct a new GraphMLKey for the input variables.
    GraphMLKey *attribute = new GraphMLKey(name, type, forString);

    //Search for a matching Key. If we find one, remove the newly created GraphMLKey
    foreach(GraphMLKey* key, keys){
        if(key->equals(attribute)){
            delete attribute;
            return key;
        }
    }

    //Protect the GraphMLKey if it meant to be protected
    if(protectedKeyNames.contains(name)){
        attribute->setDefaultProtected(true);
    }

    if(name == "type"){
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Attribute" << "Member" << "Variable";
        validValues << "Boolean" << "Byte" << "Char" << "WideChar" << "ShortInteger" << "LongInteger" << "LongLongInteger" << "UnsignedShortInteger" << "UnsignedLongInteger" << "UnsignedLongLongInteger" << "FloatNumber" << "DoubleNumber" << "LongDoubleNumber" << "GenericObject" << "GenericValue" << "GenericValueObject" << "String" << "WideString";


        attribute->appendValidValues(validValues, keysValues);
    }
    if(name == "middleware"){
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Model";
        validValues << "tao" << "rtidds" << "opensplice" << "coredx" << "tcpip" << "qpidpb" ;
        attribute->appendValidValues(validValues, keysValues);
    }

    if(type == "boolean"){
        QStringList validValues;
        validValues << "true" << "false";
        attribute->appendValidValues(validValues);
    }

    if (name == "actionOn") {
        QStringList validValues;
        QStringList keysValues;
        keysValues << "Process";
        validValues << "Activate" << "Preprocess" << "Mainprocess" << "Postprocess" << "Passivate";
        attribute->appendValidValues(validValues, keysValues);
    }

    if(name == "operation"){
        QStringList validValues;
        QStringList keysValues;
        keysValues << "VectorOperation";
        validValues << "Set" << "Get" << "Remove";
        attribute->appendValidValues(validValues, keysValues);
    }

    connect(attribute, SIGNAL(model_DisplayMessage(QString,QString,int)), this, SLOT(displayMessage(QString,QString,int)));
    //Add it to the list of GraphMLKeys.
    keys.append(attribute);
    return attribute;
}

GraphMLKey *NewController::getGraphMLKeyFromName(QString name)
{
    foreach(GraphMLKey* key, keys){
        if(key->getName() == name){
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
    if(source->canConnect(destination)){
        QString sourceKind = source->getDataValue("kind");
        QString destinationKind = destination->getDataValue("kind");

        Edge* edge = new Edge(source, destination);

        return edge;
    }else{
        if(!source->isConnected(destination)){
            //qCritical() << "Edge: Source: " << source->toString() << " to Destination: " << destination->toString() << " Cannot be created!";
        }
        return 0;
    }
}

void NewController::storeGraphMLInHash(GraphML *item)
{
    if(!item){
        return;
    }

    int ID = item->getID();
    if(IDLookupGraphMLHash.contains(ID)){
        qCritical() << "Hash already contains item with ID: " << ID;
    }else{
        IDLookupGraphMLHash[ID] = item;
        if(item->getKind() == GraphML::NODE){
            QString nodeKind = ((Node*)item)->getNodeKind();
            kindLookup[nodeKind].append(ID);
            reverseKindLookup[ID] = nodeKind;
            QString treeIndexStr = ((Node*)item)->getTreeIndexString();

            treeLookup[treeIndexStr] = ID;
            reverseTreeLookup[ID] = treeIndexStr;

            nodeIDs.append(ID);
        }else if(item->getKind() == GraphML::EDGE){
            edgeIDs.append(ID);
        }
        controller_GraphMLConstructed(item);
    }
}

GraphML *NewController::getGraphMLFromHash(int ID)
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
        GraphML* item = IDLookupGraphMLHash[ID];
        if(item)
        {
            emit controller_GraphMLDestructed(ID, item->getKind());
        }else
        {
            qCritical() << "GOT ITEM NOT IN CONTROLLER HASH";
        }

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

        IDLookupGraphMLHash.remove(ID);

        if(item->getKind() == GraphML::NODE){
            nodeIDs.removeOne(ID);
        }else if(item->getKind() == GraphML::EDGE){
            edgeIDs.removeOne(ID);
        }
        if(IDLookupGraphMLHash.size() != (nodeIDs.size() + edgeIDs.size())){
            qCritical() << "Hash Map Inconsistency detected!";
        }
    }
}

Node *NewController::constructChildNode(Node *parentNode, QList<GraphMLData *> nodeData)
{
    //Get the Kind from the data.
    QString childNodeKind = getDataValueFromKeyName(nodeData, "kind");
    QString childNodeType = getDataValueFromKeyName(nodeData, "type");
    QString childNodeLabel = getDataValueFromKeyName(nodeData, "label");


    Node* node = constructTypedNode(childNodeKind, childNodeType, childNodeLabel);

    //Enforce Default Data!
    QList<GraphMLData*> requiredData = constructGraphMLDataVector(childNodeKind);


    bool isInModel = isGraphMLInModel(node);

    if(node){
        //Attach Default Data.
        _attachGraphMLData(node, requiredData, isInModel);

        //Update Data with custom Data!
        _attachGraphMLData(node, nodeData, isInModel);
    }

    //Delete the GraphMLData objects which didn't get adopted to the Node (or if our Node is null)
    while(!nodeData.isEmpty()){
        GraphMLData* data = nodeData.takeFirst();
        if(!node || !node->containsData(data)){
            delete data;
        }
    }

    if(!node){
        qCritical() << "Node was not successfully constructed!";
        return 0;
    }


    //If we have no parentNode, attempt to attach it to the Model.
    if(!parentNode){
        parentNode = model;
    }


    if(!isInModel){
        qCritical() << node->toString();
        if(parentNode->canAdoptChild(node)){
            parentNode->addChild(node);

            //Force Unique labels and Sort Order. Can only happen after adoption.
            enforceUniqueLabel(node);
            enforceUniqueSortOrder(node);

            constructNodeGUI(node);
        }else{
            destructNode(node, false);
            return 0;
        }
    }

    //If the node is a definition, construct an instance/Implementation in each Instance/Implementation of the parentNode.
    if(node->isDefinition()){
        foreach(Node* child, parentNode->getInstances()){
            constructDefinitionRelative(child, node, true);
        }
        foreach(Node* child, parentNode->getImplementations()){
            constructDefinitionRelative(child, node, false);
        }
    }
    Parameter* param = dynamic_cast<Parameter*>(node);
    if(param){
        qCritical() << "PARAMETER";
        if(param->isInputParameter()){
            GraphMLData* paramLabel = param->getData("label");
            GraphMLData* paramValue = param->getData("value");
            if(paramLabel){
                QString paramName = paramLabel->getValue();
                GraphMLData* parentData = parentNode->getData(paramName);
                if(parentData){/*
                    qCritical() << "SETTING PARENT DATA!";
                    qCritical() << paramValue << parentData;
                    qCritical() << paramValue->getValue()  << parentData->getValue();*/
                    parentData->bindData(paramValue, false);
                    paramValue->bindData(parentData, false);

                }
            }
        }
    }
    return node;
}

QList<GraphMLData *> NewController::constructGraphMLDataVector(QString nodeKind, QPointF relativePosition)
{
    GraphMLKey* kindKey = constructGraphMLKey("kind", "string", "node");
    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "node");
    GraphMLKey* xKey = constructGraphMLKey("x", "double", "node");
    GraphMLKey* yKey = constructGraphMLKey("y", "double", "node");
    GraphMLKey* typeKey = constructGraphMLKey("type", "string", "node");
    GraphMLKey* widthKey = constructGraphMLKey("width", "double", "node");
    GraphMLKey* heightKey = constructGraphMLKey("height", "double", "node");
    GraphMLKey* sortKey = constructGraphMLKey("sortOrder", "int", "node");
    GraphMLKey* expandedKey = constructGraphMLKey("isExpanded", "boolean", "node");



    QList<GraphMLData*> data;

    QString labelString = nodeKind;
    if(nodeKind.endsWith("Parameter")){
        labelString = "";
    }


    data.append(new GraphMLData(kindKey, nodeKind));
    data.append(new GraphMLData(xKey, QString::number(relativePosition.x())));
    data.append(new GraphMLData(yKey, QString::number(relativePosition.y())));
    data.append(new GraphMLData(widthKey));
    data.append(new GraphMLData(heightKey));


    bool protectLabel = nodeKind.endsWith("Parameter");
    data.append(new GraphMLData(labelKey, labelString, protectLabel));
    data.append(new GraphMLData(sortKey,"-1"));


    //Attach Node Specific Data.

    if(nodeKind != "Model" && !nodeKind.endsWith("Definitions")){
        data.append(new GraphMLData(expandedKey, "false"));
    }

    if(nodeKind == "ManagementComponent"){
        data.append(new GraphMLData(typeKey));

    }
    if(nodeKind == "Model"){
        GraphMLKey* middlewareKey = constructGraphMLKey("middleware", "string", "node");
        GraphMLKey* projectUUID = constructGraphMLKey("projectUUID", "string", "node");

        data.append(new GraphMLData(projectUUID, getTimeStamp()));
        data.append(new GraphMLData(middlewareKey, "tao"));
    }
    if(nodeKind == "PeriodicEvent"){
        GraphMLKey* frequencyKey = constructGraphMLKey("frequency", "string", "node");
        data.append(new GraphMLData(typeKey, "Constant"));
        data.append(new GraphMLData(frequencyKey, "1.0"));
    }
    if(nodeKind == "Process"){
        GraphMLKey* codeKey = constructGraphMLKey("code", "string", "node");
        GraphMLKey* actionOnKey = constructGraphMLKey("actionOn", "string", "node");
        GraphMLKey* workerKey = constructGraphMLKey("worker", "string", "node");
        GraphMLKey* folderKey = constructGraphMLKey("folder", "string", "node");
        GraphMLKey* fileKey = constructGraphMLKey("file", "string", "node");
        GraphMLKey* operationKey = constructGraphMLKey("operation", "string", "node");
        GraphMLKey* complexityKey = constructGraphMLKey("complexity", "string", "node");
        GraphMLKey* complexityParamsKey = constructGraphMLKey("complexityParameters", "string", "node");
        GraphMLKey* parametersKey = constructGraphMLKey("parameters", "string", "node");
        data.append(new GraphMLData(codeKey));
        data.append(new GraphMLData(actionOnKey, "Mainprocess"));
        data.append(new GraphMLData(workerKey));
        data.append(new GraphMLData(complexityParamsKey));

        data.append(new GraphMLData(folderKey));
        data.append(new GraphMLData(fileKey));
        data.append(new GraphMLData(operationKey));
        data.append(new GraphMLData(complexityKey));
        data.append(new GraphMLData(parametersKey));
    }
    if(nodeKind == "Condition"){
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        data.append(new GraphMLData(valueKey));
    }
    if(nodeKind == "Member"){
        GraphMLKey* keyKey = constructGraphMLKey("key", "boolean", "node");
        data.append(new GraphMLData(keyKey, "false"));
        data.append(new GraphMLData(typeKey, "String"));
    }
    if(nodeKind == "MemberInstance"){
        GraphMLKey* keyKey = constructGraphMLKey("key", "boolean", "node");
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        data.append(new GraphMLData(valueKey));
        data.append(new GraphMLData(keyKey, "false"));
    }
    if(nodeKind == "HardwareNode"){
        GraphMLKey* osKey = constructGraphMLKey("os", "string", "node");
        GraphMLKey* osVKey = constructGraphMLKey("os_version", "string", "node");
        GraphMLKey* ipKey = constructGraphMLKey("ip_address", "string", "node");
        GraphMLKey* archKey = constructGraphMLKey("architecture", "string", "node");
        data.append(new GraphMLData(osKey));
        data.append(new GraphMLData(osVKey));
        data.append(new GraphMLData(ipKey));
        data.append(new GraphMLData(archKey));
    }
    if(nodeKind == "Attribute"){
        data.append(new GraphMLData(typeKey, "String"));
    }
    if(nodeKind == "Vector"){
        GraphMLKey* sizeKey = constructGraphMLKey("max_size", "int", "node");
        data.append(new GraphMLData(sizeKey, "0"));
        data.append(new GraphMLData(typeKey, ""));
    }

    if(nodeKind == "AttributeInstance"){
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        data.append(new GraphMLData(valueKey));
    }
    if(nodeKind == "Variable"){
        data.append(new GraphMLData(typeKey, "String"));
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        data.append(new GraphMLData(valueKey));
    }
    if(nodeKind == "OutEventPortInstance" || nodeKind == "InEventPortInstance"){
        GraphMLKey* topicKey = constructGraphMLKey("topicName", "string", "node");
        data.append(new GraphMLData(topicKey));
    }

    if(nodeKind.endsWith("Instance") ||nodeKind.endsWith("Impl")){
        data.append(new GraphMLData(typeKey));
    }

    if(nodeKind.contains("EventPort")){
        data.append(new GraphMLData(typeKey, "", true));
    }
    if(nodeKind == "InEventPort"){
        GraphMLKey* asyncKey = constructGraphMLKey("async", "boolean", "node");
        data.append(new GraphMLData(asyncKey, "true"));
    }
    if(nodeKind.endsWith("Parameter")){
        if(nodeKind == "InputParameter"){
            GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
            data.append(new GraphMLData(valueKey));
        }
        data.append(new GraphMLData(typeKey, "", true));
    }
    if(nodeKind == "VectorOperation"){
        GraphMLKey* indexKey = constructGraphMLKey("index", "string", "node");
        GraphMLKey* vectorKey = constructGraphMLKey("vector", "string", "node");
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        GraphMLKey* operationKey = constructGraphMLKey("operation", "string", "node");
        data.append(new GraphMLData(indexKey));
        data.append(new GraphMLData(vectorKey));
        data.append(new GraphMLData(valueKey));
        data.append(new GraphMLData(operationKey));
    }
    return data;
}

QString NewController::getNodeInstanceKind(Node *definition)
{
    QString definitionKind = definition->getDataValue("kind");
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
    QString definitionKind = definition->getDataValue("kind");
    QString kindModifier = "Impl";

    QString implKind = definitionKind + kindModifier;

    if(definitionKind.endsWith("Instance")){
        implKind = definitionKind;
    }

    return implKind;
}



int NewController::constructDefinitionRelative(Node *parent, Node *definition, bool isInstance)
{
    int nodesMatched = 0;
    bool requiresLabelAndType = false;
    QString definitionKind = definition->getDataValue("kind");

    if(definitionKind == "Attribute" || definitionKind == "Vector" || definitionKind.startsWith("Member")){
        requiresLabelAndType = true;
    }

    QString childKind = "";

    if(isInstance){
        childKind = getNodeInstanceKind(definition);
    }else{
        childKind = getNodeImplKind(definition);
    }


    //For each child in parent, check to see if any Nodes match Label/Type
    foreach(Node* child, parent->getChildren(0)){
        if(child->getDefinition() == definition){
            //Return the child, if we already have a definition relation.
            return 1;
        }

        if(child->getDefinition()){
            //If the child has a different definition, move onto the next child.
            continue;
        }

        bool labelMatched = false;
        bool typeMatched = false;


        //Check for kind.
        if(child->getDataValue("kind") != childKind){
            continue;
        }

        if(child->getDataValue("label") == definition->getDataValue("label")){
            labelMatched = true;
        }

        if(child->getDataValue("type") == definition->getDataValue("type")){
            typeMatched = true;
        }

        //Type can equal the label of the definition.
        if(child->getDataValue("type") == definition->getDataValue("label")){
            typeMatched = true;
        }

        if(typeMatched){
            if((requiresLabelAndType && labelMatched) || !requiresLabelAndType){
                Edge* connectingEdge = constructEdgeWithData(child, definition);

                if(!connectingEdge){
                    qCritical() << "constructDefinitionRelative(): Couldn't construct Edge between Relative Node and Definition Node.";
                    continue;
                }
                //Don't create an ActionItem for this.
                //.connectingEdge->setGenerated(true);
                nodesMatched++;
            }
        }
    }

    //If we didn't find a match, we must create an Instance.
    if(nodesMatched == 0){
        Node *instanceNode = constructChildNode(parent, constructGraphMLDataVector(childKind));

        if(!instanceNode){
            if(!parent->getDataValue("kind").endsWith("EventPortInstance")){
                qCritical() << "constructDefinitionRelative(): Couldn't construct a Relative Node.";
            }
            return 0;
        }

        Edge* connectingEdge = constructEdgeWithData(instanceNode, definition);

        if(!connectingEdge){
            qCritical() << "constructDefinitionRelative() : Couldn't construct Edge between Relative Node and Definition Node.";
            return 0;
        }
        //Don't create an ActionItem for this.
        instanceNode->setGenerated(true);
        connectingEdge->setGenerated(true);

        nodesMatched ++;
    }

    return nodesMatched;
}




void NewController::enforceUniqueLabel(Node *node, QString nodeLabel)
{
    QString toSetLabel = nodeLabel;
    QString currentLabel = node->getDataValue("label");
    if(!node){
        return;
    }

    Node* parentNode = node->getParentNode();

    if(nodeLabel == ""){
        nodeLabel = node->getDataValue("label");
    }
    if(node->getDataValue("kind") == "Process"){
        node->updateDataValue("label", nodeLabel);
        return;
    }
    if(node->getDataValue("kind") == "Process"){
        node->updateDataValue("label", nodeLabel);
        return;
    }



    int maxNumber = -1;
    QList<int> sameLabelNumbers;

    //If we have no parent node we don't need to enforce unique labels.
    if(parentNode){
        foreach(Node* siblingNode, parentNode->getChildren(0)){
            if(siblingNode == node){
                //Don't force uniquity on self.
                continue;
            }
            QString siblingLabel = siblingNode->getDataValue("label");

            if(siblingLabel == nodeLabel){
                sameLabelNumbers << 0;
                if(0 > maxNumber){
                    maxNumber = 0;
                }
            }else if(siblingLabel.startsWith(nodeLabel)){
                //If sibling's label begins with nodeLabel, check for underscores and numbers for uniquity.
                QString labelRemainder = siblingLabel.right(siblingLabel.size() - nodeLabel.size());

                if(labelRemainder.startsWith("_") && labelRemainder.count("_") == 1){
                    labelRemainder = labelRemainder.right(labelRemainder.size() - 1);

                    bool isInt;
                    int number = labelRemainder.toInt(&isInt);
                    if(isInt && number > 0){
                        if(number > maxNumber){
                            maxNumber = number;
                        }
                        sameLabelNumbers << number;
                    }
                }
            }
        }
    }
    int labelID = 0;


    for(labelID=0; labelID <= maxNumber; labelID++){
        if(!sameLabelNumbers.contains(labelID)){
            break;
        }
    }

    if(labelID > 0){
        nodeLabel += QString("_%1").arg(labelID);
    }


    node->updateDataValue("label", nodeLabel);
    if(currentLabel == nodeLabel && nodeLabel != toSetLabel){
        GraphMLData* labelData = node->getData("label");
        emit labelData->dataChanged(labelData);
    }
}

void NewController::enforceUniqueSortOrder(Node *node, int newPosition)
{
    if(!node){
        return;
    }

    Node* parentNode = node->getParentNode();

    if(parentNode){
        int maxSortOrder = parentNode->childrenCount() -1;

        int originalPosition = node->getDataNumberValue("sortOrder");


        //Got original Position.
        if(originalPosition == -1){
            originalPosition = parentNode->childrenCount() - 1;
        }

        //If newPosition == -1 and originalPosition == -1 we should set the sort Order to the next child size.
        if(newPosition == -1){
            newPosition = originalPosition;
        }


        //Bound the new value for sortOrder based on the parent size.
        if(newPosition >= parentNode->childrenCount()){
            newPosition = parentNode->childrenCount() - 1;
        }

        //Bound the new value for sortOrder base on minimum  children
        if(newPosition < 0){
            newPosition = 0;
        }

        int lowerPos = qMin(originalPosition, newPosition);
        int upperPos = qMax(originalPosition, newPosition);

        //If we are updating. refactor.
        if(originalPosition == newPosition){
            lowerPos = originalPosition;
            upperPos = maxSortOrder;
        }

        int modifier = 1;
        if(newPosition > originalPosition){
            modifier = -1;
        }


        foreach(Node* sibling, node->getSiblings()){
            int currentPos = sibling->getDataNumberValue("sortOrder");

            if(currentPos >= lowerPos && currentPos <= upperPos){
                sibling->updateDataValue("sortOrder", currentPos + modifier);
            }
        }
    }
    GraphMLData* sortData = node->getData("sortOrder");
    sortData->setValue(newPosition);
    emit sortData->dataChanged(sortData);
}

bool NewController::destructNode(Node *node, bool addAction)
{
    if(!node){
        qCritical() << "NewController::destructNode() Got NULL Node!";
        return true;
    }
    if(addAction){
        if(!canDeleteNode(node)){
            return false;
        }
    }

    //Gotta Delete in Order.
    QString XMLDump = "";
    int ID = node->getID();
    int parentID = -1;
    if(node->getParentNode()){
        parentID = node->getParentNode()->getID();
    }

    bool toInt;
    int nodePos = node->getDataValue("sortOrder").toInt(&toInt);
    //Update sort order for silbings.
    if(toInt){
        foreach(Node* sibling, node->getSiblings()){
            int siblingPos = sibling->getDataValue("sortOrder").toInt(&toInt);
            if(toInt){
                if(siblingPos > nodePos){
                    sibling->updateDataValue("sortOrder", QString::number(siblingPos-1));
                }
            }
        }
    }

    if(addAction){
        //Export only if we are add this node to reverse state.
        XMLDump = _exportGraphMLDocument(node);
    }

    //Only for top parent, DELETE ALL EDGES for everything.

    while(node->edgeCount() > 0){
        Edge* edge = node->getEdges(0).last();
        if(edge){
            //qCritical() << "Deleting Edge: " << edge->toString();
            if(!(node->isAncestorOf(edge->getDestination()) && node->isAncestorOf(edge->getSource()))){
                //Add an Undo Step for things not completly owned by this.
                destructEdge(edge, true);
            }
        }else{
            destructEdge(edge, false);
        }
    }

    while(node->childrenCount() > 0){
        Node* child = node->getChildren(0).takeFirst();
        if(child){
            destructNode(child, false);
        }
    }

    if(!node->wasGenerated() || addAction){
        //Add an action to reverse this action.
        ActionItem action;
        action.actionKind = GraphML::NODE;
        action.actionType = DESTRUCTED;
        action.removedXML = XMLDump;
        action.ID = ID;
        if(node->getParentNode()){
            action.parentID = node->getParentNode()->getID();
        }
        //qCritical() << "DESTRUCTING Node:" << node->toString();
        addActionToStack(action, addAction);
    }

    removeGraphMLFromHash(ID);


    HardwareNode* hNode = dynamic_cast<HardwareNode*>(node);
    HardwareCluster* hCNode = dynamic_cast<HardwareCluster*>(node);
    ManagementComponent* mCNode = dynamic_cast<ManagementComponent*>(node);
    if(hNode){
        QString nodeName = hNode->getDataValue("label");
        hardwareNodes.remove(nodeName);
    }
    if(hCNode){
        QString nodeName = hCNode->getDataValue("label");
        hardwareClusters.remove(nodeName);
    }
    if(mCNode){
        QString nodeName = mCNode->getDataValue("label");
        managementComponents.remove(nodeName);
    }


    //node->deleteLater();
    delete node;
    //delete node;
    return true;
}










bool NewController::destructEdge(Edge *edge, bool addAction)
{
    if(!edge){
        qCritical() << "destructEdge(): Edge is NULL";
        return true;
    }


    //Get information about the edge.
    int ID = edge->getID();
    int srcID = -1;
    int dstID = -1;
    Node* source = edge->getSource();

    Node* destination = edge->getDestination();

    if(!(source && destination)){
        qCritical() << "destructEdge(): Source and/or Destination are NULL.";
        return false;
    }
    srcID = source->getID();
    dstID = destination->getID();

    //If the Edge Wasn't Generated, and we are meant to add an Action for this removal, Add an undo state.
    if(!edge->wasGenerated() || addAction){
        ActionItem action;
        action.actionType = DESTRUCTED;
        action.actionKind = GraphML::EDGE;
        action.ID = ID;
        action.srcID = source->getID();
        action.dstID = destination->getID();
        //Serialize data values.
        foreach(GraphMLData* data, edge->getData()){
            action.dataValues.append(data->toStringList());
        }
        //qCritical() << "DESTRUCTING EDGE:" << edge->toString();

        addActionToStack(action, addAction);
    }

    //Node* toDeleteNode = 0;
    //Delete the Instan
    if(edge->isInstanceLink()){
        //If Edge represents an Instance relationship; Tear it down.
        teardownDefinitionRelationship(destination, source, SETUP_AS_INSTANCE);
    }else if(edge->isImplLink()){
        //If Edge represents an Implementation relationship; Tear it down.
        teardownDefinitionRelationship(destination, source, SETUP_AS_IMPL);
    }else if(edge->isAggregateLink()){
        EventPort* eventPort = dynamic_cast<EventPort*>(source);
        Vector* vector = dynamic_cast<Vector*>(source);
        Aggregate* aggregate = dynamic_cast<Aggregate*>(destination);
        if(eventPort && aggregate){
            teardownAggregateRelationship(eventPort, aggregate);
        }else if(vector && aggregate){
            teardownVectorRelationship(vector, aggregate);
        }else{
            qCritical() << "Unknown AggregateLink!";
        }
    }else if(edge->isComponentLink()){
        // UnBind Topics Together.
        GraphMLData* sourceTopicName = source->getData("topicName");
        GraphMLData* destinationTopicName = destination->getData("topicName");
        if(destinationTopicName && sourceTopicName ){
            sourceTopicName->unbindData(destinationTopicName);
        }
    }else if(edge->isDelegateLink()){
        Node* del = destination;
        if(source->getNodeKind().contains("Delegate")){
            del = source;
        }
        if(del){
            GraphMLData* typeData = del->getData("type");
            if(typeData){
                typeData->unsetParentData();
                typeData->clearValue();
            }
        }
    }




    //Remove it from the hash of GraphML
    removeGraphMLFromHash(ID);

    //Delete it.
    //delete edge;
    delete edge;//edge->deleteLater();

    return true;
}



bool NewController::isEdgeLegal(Node *src, Node *dst)
{
    if(src && dst){
        //Check for dual way connections.
        return src->canConnect(dst);
    }
    return false;
}

bool NewController::isNodeKindImplemented(QString nodeKind)
{
    return containerNodeKinds.contains(nodeKind) || constructableNodeKinds.contains(nodeKind);
}

bool NewController::reverseAction(ActionItem action)
{
    //Switch on the Action Type.
    switch(action.actionType){
    case CONSTRUCTED:{


        switch(action.actionKind){
        case GraphML::NODE:{
            //Delete Node.
            Node* node = getNodeFromID(action.ID);
            if(node){
                return destructNode(node);
            }
            break;
        }
        case GraphML::EDGE:{
            //Delete Edge.
            Edge* edge = getEdgeFromID(action.ID);
            if(edge){
                return destructEdge(edge, true);
            }
            break;
        }
        case GraphML::DATA:{
            //Delete Data
            GraphML* item = getGraphMLFromID(action.ID);

            if(item){
                destructGraphMLData(item, action.keyName);
                if(!item->getData(action.keyName)){
                    return true;
                }
            }else{
                qCritical() << "\ncase CONSTRUCTED:GraphML::DATA Cannot find Item";
                qCritical() << "action.ID: " << action.ID;
                return false;
            }

            break;
        }
        default:{
            break;
        }
        }
        break;
    }

    case DESTRUCTED:{
        switch(action.actionKind){
        case GraphML::NODE:{
            //qCritical() << "Redo: Delete Node" << action.ID;
            //Get Parent Node, and Construct Node.
            Node* parentNode = getNodeFromID(action.parentID);
            if(parentNode){
                return _importGraphMLXML(action.removedXML, parentNode, true, false);
            }else{
                qCritical() << "Cannot find Node";
                return false;
            }
            break;
        }
        case GraphML::EDGE:{
            //Get Source and Destination, attempt to construct an Edge.

            Node* src = getNodeFromID(action.srcID);
            Node* dst = getNodeFromID(action.dstID);

            //qCritical() << "Redo: Delete Edge Between: " << action.srcID << " AND " << action.dstID;


            if(src && dst){
                if(src->isConnected(dst)){
                    return true;
                }
                if(isEdgeLegal(src,dst)){
                    constructEdgeWithData(src ,dst, action.dataValues, action.ID);
                    if(src->isConnected(dst)){
                        return true;
                    }
                }else{
                    return false;
                }
            }else{
                if(!src){
                    qCritical() << "Cannot find src GraphML" << action.srcID;
                }
                if(!dst){
                    qCritical() << "Cannot find dst GraphML" << action.dstID;

                }
            }
            break;
        }
        case GraphML::DATA:{

            GraphML* attachedItem = getGraphMLFromID(action.ID);

            if(attachedItem){
                bool success = _attachGraphMLData(attachedItem, action.dataValues);
                if(!success){
                    qCritical() << "Could not Attach GraphMLData";
                }
                return success;
            }else{

                qCritical() << "Destructed Data: Cannot find Item";
            }
            break;
        }
        default:{
            break;
        }
        }
        break;



    }
    case MODIFIED:{
        switch(action.actionKind){
        case GraphML::DATA:{
            GraphML* attachedItem = getGraphMLFromID(action.ID);

            if(attachedItem){
                if(action.isNum){
                    //Restore the Data Value;
                    setGraphMLData(attachedItem, action.keyName, action.dataValueNum);
                }else{
                    //Restore the Data Value;
                    setGraphMLData(attachedItem, action.keyName, action.dataValue);

                }
                return true;
            }else{
                //if(!IS_SUB_VIEW){
                return false;
                //}
            }


            break;
        }
        default:{
            break;
        }
        }
        break;
    }
    }
    return false;

}
bool NewController::_attachGraphMLData(GraphML *item, QList<QStringList> dataList, bool addAction)
{
    QList<GraphMLData*> graphMLDataList;
    //Conver the StringList into GraphMLData Objects.

    foreach(QStringList data, dataList){
        if(data.size() != 5){
            qCritical() << "GraphMLData Cannot be Parsed.";
            continue;
        }

        QString keyName = data.at(0);
        QString keyType = data.at(1);
        QString keyFor = data.at(2);
        QString dataValue = data.at(3);
        bool isProtected = data.at(4) == "true";

        GraphMLKey* key = constructGraphMLKey(keyName, keyType, keyFor);
        if(!key){
            qCritical() << "Cannot Construct Key";
            continue;
        }

        GraphMLData* graphMLData = new GraphMLData(key, dataValue);
        if(!graphMLData){
            qCritical() << "Cannot Construct GraphMLData";
            continue;
        }

        graphMLData->setProtected(isProtected);
        graphMLDataList.append(graphMLData);
    }

    return _attachGraphMLData(item, graphMLDataList, addAction);
}

bool NewController::_attachGraphMLData(GraphML *item, QList<GraphMLData *> dataList, bool addAction)
{
    if(!item){
        return false;
    }

    foreach(GraphMLData* data, dataList){
        GraphMLKey* currentKey = data->getKey();
        //Check if the item has a GraphMLData already.
        if(item->getData(currentKey)){
            setGraphMLData(item, data->getKeyName(), data->getValue(), addAction);
        }else{
            attachGraphMLData(item, data, addAction);
        }

        GraphMLData* updateData = item->getData(currentKey);
        if(updateData && !updateData->isProtected()){
            updateData->setProtected(data->isProtected());
        }
    }
    return true;
}

bool NewController::_attachGraphMLData(GraphML *item, GraphMLData *data, bool addAction)
{
    QList<GraphMLData *> dataList;
    dataList.append(data);
    return _attachGraphMLData(item, dataList, addAction);
}

void NewController::addActionToStack(ActionItem action, bool useAction)
{
    //Get Current Action ID and action.
    action.actionID = currentActionID;
    action.actionName = currentAction;
    action.timestamp = getTimeStamp();
    action.actionItemID = currentActionItemID++;

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
    if(undo){
        UNDOING = true;
        REDOING = false;
    }else{
        REDOING = true;
        UNDOING = false;
    }


    //Used to store the stack of actions we are to use.
    QStack<ActionItem> actionStack = redoActionStack;

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
    int topActionID = actionStack.top().actionID;
    QString topActionName = actionStack.top().actionName;

    //Emit a new action so this Undo/Redo operation can be reverted.
    triggerAction(topActionName);

    //This vector will store all ActionItems which match topActionID
    QList<ActionItem> toReverse;
    while(!actionStack.isEmpty()){
        //Get the top-most action.
        ActionItem action = actionStack.top();


        //If this action has the same ID, we should undo it.
        if(action.actionID == topActionID){
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
        ActionItem reverseState = toReverse.takeFirst();
        bool success = reverseAction(reverseState);
        if(!success){

            retryCount[reverseState.actionItemID] +=1;
            if(retryCount[reverseState.actionItemID] <= maxRetry){
                toReverse.append(reverseState);
            }

        }else{
            actionsReversed ++;
            int percentage = (actionsReversed*100) / actionCount;
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

void NewController::logAction(ActionItem item)
{
    if(DELETING){
        return;
    }


    QTextStream out(logFile);

    //QString
    QString actionType;
    QString actionKind;
    QString extraInfo = "";

    if(item.actionType == CONSTRUCTED){
        actionType = "C";
    }
    if(item.actionType == DESTRUCTED){
        actionType = "D";
    }
    if(item.actionType == MODIFIED){
        actionType = "M";
    }

    if(item.actionKind == GraphML::NODE){
        actionKind = "NODE";
        extraInfo += "\t\tLabel: '" + item.itemLabel + '"';
        extraInfo += "\t\tKind: '" + item.itemKind + '"';


    }
    if(item.actionKind == GraphML::EDGE){
        actionKind = "EDGE";
        //if(item.actionType == CONSTRUCTED){
        extraInfo += "\t\tSRC_ID: '" + item.srcID + '"';
        extraInfo += "\t\tDST_ID: '" + item.dstID + '"';
        //}

    }
    if(item.actionKind == GraphML::DATA){
        actionKind = "DATA";
        extraInfo += "\t\tKey: '" + item.keyName +"'";
        if(item.actionType == MODIFIED){
            extraInfo += "\t\tPrevious Value: '" + item.dataValue + '"';
        }
    }


    out << item.timestamp << "\t" << item.actionItemID << "\t" << actionType << "\t" << actionKind << "\t" << item.ID;
    if(extraInfo != ""){
        out << extraInfo;
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

void NewController::clearHistory()
{
    currentActionID = 0;
    actionCount = 0;
    currentAction = "";
    undoActionStack.clear();
    redoActionStack.clear();
    updateUndoRedoState();
}

Node *NewController::constructTypedNode(QString nodeKind, QString nodeType, QString nodeLabel)
{
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
    }else if(nodeKind == "HardwareNode"){
        if(hardwareNodes.contains(nodeLabel)){
            return hardwareNodes[nodeLabel];
        }else{
            HardwareNode* hN = new HardwareNode();
            hardwareNodes[nodeLabel] = hN;
            return hN;
        }
    }else if(nodeKind == "HardwareCluster"){
        if(hardwareClusters.contains(nodeLabel)){
            return hardwareClusters[nodeLabel];
        }else{
            HardwareCluster* hC = new HardwareCluster();
            hardwareClusters[nodeLabel] = hC;
            return hC;
        }
    }else if(nodeKind == "ManagementComponent"){
        if(managementComponents.contains(nodeType)){
            return managementComponents[nodeType];
        }else{
            ManagementComponent* mC = new ManagementComponent();
            managementComponents[nodeLabel] = mC;
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
    }else if(nodeKind == "VectorOperation"){
        return new VectorOperation();
    }else{
        qCritical() << "Node Kind:" << nodeKind << " not yet implemented!";
        return new BlankNode();
    }

    return 0;
}

void NewController::constructNodeGUI(Node *node)
{
    if(!node){
        qCritical() << "Cannot Construct Node GUI element. Null Node.";
        return;
    }

    //Construct an ActionItem to reverse Node Construction.
    ActionItem action;
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::NODE;
    action.ID = node->getID();
    action.itemLabel = node->getDataValue("label");
    action.itemKind = node->getDataValue("kind");

    if(node->getParentNode()){
        //Set the ParentNode ID if we have a Parent.
        action.parentID = node->getParentNode()->getID();
    }



    //Add the node to the list of nodes constructed.
    storeGraphMLInHash(node);

    //Add Action to the Undo/Redo Stack.
    addActionToStack(action);
}

void NewController::setupModel()
{
    model = dynamic_cast<Model*>(constructTypedNode("Model"));
    _attachGraphMLData(model, constructGraphMLDataVector("Model"));
    constructNodeGUI(model);

    GraphMLData* labelData = model->getData("label");
    connect(labelData, SIGNAL(valueChanged(QString)), this, SIGNAL(controller_ProjectNameChanged(QString)));

    //Construct the top level parents.
    interfaceDefinitions = constructChildNode(model, constructGraphMLDataVector("InterfaceDefinitions"));
    behaviourDefinitions = constructChildNode(model, constructGraphMLDataVector("BehaviourDefinitions"));
    deploymentDefinitions =  constructChildNode(model, constructGraphMLDataVector("DeploymentDefinitions"));

    //Construct the second level containers.
    assemblyDefinitions =  constructChildNode(deploymentDefinitions, constructGraphMLDataVector("AssemblyDefinitions"));
    hardwareDefinitions =  constructChildNode(deploymentDefinitions, constructGraphMLDataVector("HardwareDefinitions"));

    protectedNodes << model;
    protectedNodes << interfaceDefinitions;
    protectedNodes << behaviourDefinitions;
    protectedNodes << deploymentDefinitions;
    protectedNodes << assemblyDefinitions;
    protectedNodes << hardwareDefinitions;


    setupManagementComponents();
    setupLocalNode();
    //Clear the Undo/Redo Stacks
    undoActionStack.clear();
    redoActionStack.clear();

    //Update the GUI
    emit labelData->valueChanged(labelData->getValue());
}



void NewController::bindGraphMLData(Node *definition, Node *child)
{
    if(!(definition && child)){
        return;
    }

    GraphMLData* def_Type = definition->getData("type");
    GraphMLData* def_Label = definition->getData("label");
    GraphMLData* child_Type = child->getData("type");
    GraphMLData* child_Label = child->getData("label");
    GraphMLData* def_Key = definition->getData("key");
    GraphMLData* child_Key = child->getData("key");
    GraphMLData* def_Sort = definition->getData("sortOrder");
    GraphMLData* child_Sort = child->getData("sortOrder");

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
        def_Type->bindData(child_Type);
    }else{
        def_Label->bindData(child_Type);
    }
    child_Type->setProtected(true);

    if(def_Key && child_Key){
        def_Key->bindData(child_Key);
    }

    if(bindSort){
        def_Sort->bindData(child_Sort);
    }

    if(bindLabels){
        def_Label->bindData(child_Label);
    }else{
        //Set the value.
        if(child_Label->getValue().startsWith(child->getDataValue("kind"))){
            if(child->isImpl()){
                setGraphMLData(child, "label", def_Label->getValue() + "_Impl");
            }else{
                setGraphMLData(child, "label", def_Label->getValue() + "_Inst");
            }
        }
    }
}

void NewController::unbindGraphMLData(Node *definition, Node *instance)
{
    foreach(GraphMLData* attachedData, definition->getData()){
        GraphMLData* newData = 0;
        newData = instance->getData(attachedData->getKey());
        if(newData){
            if(newData->getParentData() == attachedData){
                attachedData->unbindData(newData);
            }
        }
    }
}

/**
 * @brief NewController::setupDefinitionRelationship
 * Attempts to construct/set the provided node as an Instance of the definition provided.
 * Will Adopt Instances of all Definitions contained by Definition provided. Binds relevant GraphMLData together.
 * @param definition - The Node which is the Definition of the relationship.
 * @param aggregate - The Node which is to be set as the Instance.
 * @param instance - Is this an Instance or Implementation Relationship.
 * @return true if Definition Relation was setup correctly.
 */
bool NewController::setupDefinitionRelationship(Node *definition, Node *node, bool instance)
{
    //Got Aggregate Edge.
    if(!(definition && node)){
        qCritical() << "setupDefinitionRelationship(): Definition or Node is NULL.";
        return false;
    }

    //For each child contained in the Definition, which itself is a definition, construct an Instance/Impl inside the Parent Instance/Impl.
    foreach(Node* child, definition->getChildren(0)){
        if(child && child->isDefinition()){
            if(!node->getDataValue("kind").endsWith("EventPortInstance")){
                //Construct relationships between the children which matched the definitionChild.
                int instancesConnected = constructDefinitionRelative(node, child, instance);

                if(instancesConnected == 0){
                    qCritical() << child;
                    qCritical() << "setupDefinitionRelationship(): Couldn't create a Definition Relative for: " << child->toString() << " In: " << node->toString();
                    return false;
                }
            }
        }
    }

    //If we have made all children, we can set this node as an instance of the definition.

    //Bind the un-protected GraphMLData attached to the Definition to the Instance.
    bindGraphMLData(definition, node);

    //Get Connecting Edge.
    Edge* edge = node->getConnectingEdge(definition);
    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "edge");
    QString definitionType = "";

    //Attach the relation to the Definition.
    if(instance){
        definition->addInstance(node);
        definitionType = "Instance";
    }else{
        definition->addImplementation(node);
        definitionType = "Implementation";
    }

    if(!edge){
        qCritical() << "setupDefinitionRelationship(): Cannot find connecting Edge.";
        return false;
    }

    //Attach Data onto Edge to describe Relationship.
    if(!edge->getData(labelKey)){
        GraphMLData* label = new GraphMLData(labelKey, "Is " + definitionType + " Of");
        attachGraphMLData(edge, label, false);
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
bool NewController::setupAggregateRelationship(EventPort *eventPort, Aggregate *aggregate)
{
    //Got Aggregate Edge.
    if(!(eventPort && aggregate)){
        qCritical() << "setupAggregateRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    Node* aggregateInstance = 0;

    //Check for an Existing AggregateInstance in the EventPort.
    foreach(Node* child, eventPort->getChildren(0)){
        if(child->getDataValue("kind") == "AggregateInstance"){
            aggregateInstance = child;
        }
    }

    //If we couldn't find an AggregateInstance in the EventPort, construct one.
    if(!aggregateInstance){
        aggregateInstance = constructChildNode(eventPort, constructGraphMLDataVector("AggregateInstance"));
        aggregateInstance->setGenerated(true);
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
    Edge* edge = aggregateInstance->getConnectingEdge(aggregate);

    if(!edge){
        //Construct an Edge between the AggregateInstance an Aggregate
        constructEdgeWithData(aggregateInstance, aggregate);
        edge = aggregateInstance->getConnectingEdge(aggregate);
        //Set it is AutoGenerated as to not make an undo state for it.
		if(edge){
			edge->setGenerated(true);
        }
    }

    if(!edge){
        qCritical() << "setupAggregateRelationship(): Edge between AggregateInstance and Aggregate wasn't constructed!";
        return false;
    }

    //Check for a connecting Edge between the eventPort and aggregate.
    edge = eventPort->getConnectingEdge(aggregate);
    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "edge");

    //Check for the existance of the Edge constructed.
    if(!edge){
        qCritical() << "setupAggregateRelationship(): Edge between EventPort and Aggregate doesn't exist!";
        return false;
    }

    //Set Label of Edge.
    if(!edge->getData(labelKey)){
        GraphMLData* label = new GraphMLData(labelKey, "Uses Aggregate");
        attachGraphMLData(edge, label, false);
    }
    //Set AutoGenerated.


    //Edge Created Set Aggregate relation.
    eventPort->setAggregate(aggregate);

    //Set Type
    GraphMLData* eventPortType = eventPort->getData("type");
    GraphMLData* aggregateLabel = aggregate->getData("label");

    if(eventPortType && aggregateLabel){
        aggregateLabel->bindData(eventPortType);
        eventPortType->setValue(aggregateLabel->getValue());
    }


    return true;
}

bool NewController::teardownAggregateRelationship(EventPort *eventPort, Aggregate *aggregate)
{
    if(!(eventPort && aggregate)){
        qCritical() << "teardownAggregateRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Unset the Aggregate
    eventPort->unsetAggregate();

    //Unset Type information;
    GraphMLData* eventPortType = eventPort->getData("type");
    if(eventPortType){
        eventPortType->unsetParentData();
        eventPortType->clearValue();
    }

    QList<Node*> aggregateInstances = eventPort->getChildrenOfKind("AggregateInstance", 0);
    if(aggregateInstances.size() == 1){
        Node* child = aggregateInstances[0];
        if(child){
            //Add the AggregateInstance to the list of Nodes to delete.
            connectedLinkedIDs.append(child->getID());
        }else{
            return false;
        }
    }else{
        return false;
    }

    return true;
}

bool NewController::setupVectorRelationship(Vector *vector, Aggregate *aggregate)
{
    if(!(vector && aggregate)){
        qCritical() << "setupVectorRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Check for a connecting Edge between the eventPort and aggregate.
    Edge* edge = vector->getConnectingEdge(aggregate);


    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "edge");

    //Check for the existance of the Edge constructed.
    if(!edge){
        qCritical() << "setupVectorRelationship(): Edge between Vector and Aggregate doesn't exist!";
        return false;
    }

    //Set Label of Edge.
    if(!edge->getData(labelKey)){
        GraphMLData* label = new GraphMLData(labelKey, "Uses Aggregate");
        attachGraphMLData(edge, label, false);
    }
    //Set AutoGenerated.


    //Set Type
    GraphMLData* vectorType = vector->getData("type");
    GraphMLData* aggregateLabel = aggregate->getData("label");

    if(vectorType && aggregateLabel){
        aggregateLabel->bindData(vectorType);
        vectorType->setValue(aggregateLabel->getValue());
    }else{
        return false;
    }

    return true;

}

bool NewController::teardownVectorRelationship(Vector *vector, Aggregate *aggregate)
{
    if(!(vector && aggregate)){
        qCritical() << "teardownVectorRelationship(): EventPort or Aggregate is NULL.";
        return false;
    }

    //Unset Type information;
    GraphMLData* vectorType = vector->getData("type");
    if(vectorType){
        vectorType->unsetParentData();
        vectorType->clearValue();
    }

    return true;
}

/**
 * @brief NewController::teardownDefinitionRelationship
 * Attempts to destruct the relationship between the Instance and definition provided.
 * Will remove *ALL* Instances of all Definitions contained by Definition provided. Unbinds relevant GraphMLData and will add Node to be removed.
 * @param definition - The Node which is the Definition of the relationship.
 * @param aggregate - The Node which is the Instance.
 * @param instance - Is this an Instance or Implementation Relationship.
 * @return true if Definition Relation was removed correctly.
 */
bool NewController:: teardownDefinitionRelationship(Node *definition, Node *node, bool instance)
{
    //Got Aggregate Edge.
    if(!(definition && node)){
        qCritical() << "teardownDefinitionRelationship(): Definition or Node is NULL.";
        return false;
    }

    //Unbind data.
    unbindGraphMLData(definition, node);

    if(!definition->isConnected(node)){
        qCritical() << "teardownDefinitionRelationship(): No Edge between Definition and Node.";
        return false;
    }

    //Unset the Relationship between Definition and Instance/Impl
    if(instance){
        definition->removeInstance(node);
    }else{
        definition->removeImplementation(node);
    }

    //Remove Instance Node, by placing it in the selected Edges list.


    connectedLinkedIDs.append(node->getID());

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
            qCritical() << inputGraphML;
            return false;
        }
    }
    return true;
}

void NewController::constructEdgeGUI(Edge *edge)
{
    //Construct an ActionItem to reverse an Edge Construction.
    ActionItem action;
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::EDGE;
    action.ID = edge->getID();

    GraphMLKey* descriptionKey = constructGraphMLKey("description", "string", "edge");


    //Get Source and Destination of the Edge.
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    if(src && dst){
        action.srcID = src->getID();
        action.dstID = dst->getID();
    }

    //Add Action to the Undo/Redo Stack
    if(!edge->wasGenerated()){
        addActionToStack(action);
    }



    if(!src || !dst){
        qCritical() << "Source and Desitnation null";
    }
    //qWarning() << "Setting Up Edge: " << "Source: " << src->toString() << " to Destination: " << dst->toString();

    //Check for the special Edges
    if(dst->isDefinition()){
        //Get the Node Kind of the Source and Destination
        QString srcKind = src->getDataValue("kind");
        QString dstKind = dst->getDataValue("kind");

        if(srcKind == "" || dstKind == ""){
            qCritical() << "Got an undefined Node Kind.";
        }

        if((srcKind == dstKind) || (dstKind + "Instance" == srcKind)){
            //If the source and destination are the same type, it must be an Aggregate Instance or MemberInstance.
            setupDefinitionRelationship(dst, src, SETUP_AS_INSTANCE);
        }else if(dstKind + "Impl" == srcKind){
            //Got Implementation Edge
            setupDefinitionRelationship(dst, src, SETUP_AS_IMPL);
        }else if(edge->isAggregateLink()){
            EventPort* eventPort = dynamic_cast<EventPort*>(src);
            Vector* vector = dynamic_cast<Vector*>(src);
            Aggregate* aggregate = dynamic_cast<Aggregate*>(dst);
            if(eventPort && aggregate){
                setupAggregateRelationship(eventPort, aggregate);
            }else if(vector && aggregate){
                setupVectorRelationship(vector, aggregate);
            }else{
                qCritical() << "Unknown AggregateLink!";
            }
        }
    }
    if(src->getNodeKind().contains("PortDelegate") || dst->getNodeKind().contains("PortDelegate")){
        //PORT DELEGATE.
        Node* delegate = 0;
        Node* portInstance = 0;
        if(src->getNodeKind() == "OutEventPortInstance" && dst->getNodeKind() == "OutEventPortDelegate"){
            delegate = dst;
            portInstance = src;
        }else if(src->getNodeKind() == "InEventPortDelegate" && dst->getNodeKind() == "InEventPortInstance"){
            delegate = src;
            portInstance = dst;
        }

        if(delegate && portInstance){
            GraphMLData* delType = delegate->getData("type");
            GraphMLData* portType = portInstance->getData("type");
            if(portType && delType){
                portType->bindData(delType);
                portType->setValue(delType->getValue());
            }
        }
    }

    if(edge->isComponentLink()){
        //Bind Topics Together, if they contain.
        GraphMLData* destinationTopicName = dst->getData("topicName");
        GraphMLData* sourceTopicName = src->getData("topicName");

        if(destinationTopicName && sourceTopicName ){
            sourceTopicName->bindData(destinationTopicName);
        }

        if(!edge->getData(descriptionKey)){
            GraphMLData* label = new GraphMLData(descriptionKey, "Connected To");
            attachGraphMLData(edge, label, false);
        }
    }

    if(edge->isDataLink()){
        GraphMLData* parameterValue = src->getData("value");
        GraphMLData* dataValue = dst->getData("label");

        if(parameterValue && dataValue ){
            dataValue->bindData(parameterValue);
        }
    }

    storeGraphMLInHash(edge);
}


void NewController::setupManagementComponents()
{
    //EXECUTION MANAGER
    QList<GraphMLData*> executionManagerData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> dancePlanLauncherData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> ddsLoggingServerData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> qpidBrokerData = constructGraphMLDataVector("ManagementComponent") ;

    foreach(GraphMLData* data, executionManagerData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("DANCE_EXECUTION_MANAGER");
            data->setProtected(true);
        }
    }
    foreach(GraphMLData* data, dancePlanLauncherData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("DANCE_PLAN_LAUNCHER");
            data->setProtected(true);
        }
    }

    foreach(GraphMLData* data, ddsLoggingServerData){
        if(data->getKeyName() == "type" || data->getKeyName() == "label"){
            data->setValue("DDS_LOGGING_SERVER");
            data->setProtected(true);
        }
    }

    foreach(GraphMLData* data, qpidBrokerData){
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
    QList<GraphMLData*> localNodeData = constructGraphMLDataVector("HardwareNode") ;

    GraphMLKey* localhostKey = constructGraphMLKey("localhost", "boolean", "node");
    localNodeData.append(new GraphMLData(localhostKey ,"true", true));




    foreach(GraphMLData* data, localNodeData){
        QString keyName = data->getKeyName();
        if(keyName == "label"){
            data->setValue("localhost");
            data->setProtected(true);
        }else if(keyName == "ip_address"){
            data->setValue("127.0.0.1");
            data->setProtected(true);
        }else if(keyName == "os"){
            data->setValue(getSysOS());
            data->setProtected(true);
        }else if(keyName == "os_version"){
            data->setValue(getSysOSVersion());
            data->setProtected(true);
        }else if(keyName == "architecture"){
            data->setValue(getSysArch());
            data->setProtected(true);
        }
    }

    localhostNode = constructChildNode(hardwareDefinitions, localNodeData);
    protectedNodes << localhostNode;
}

GraphML *NewController::getGraphMLFromID(int ID)
{
    //Check for old IDs
    ID = getIDFromOldID(ID);

    GraphML* graphML = getGraphMLFromHash(ID);
    return graphML;
}

Node *NewController::getNodeFromID(int ID)
{
    GraphML* graphML = getGraphMLFromID(ID);
    return getNodeFromGraphML(graphML);
}

Node *NewController::getFirstNodeFromList(QList<int> IDs)
{
    Node* node = 0;
    foreach(int ID, IDs){
        GraphML* graphML = getGraphMLFromID(ID);
        if(graphML && graphML->isNode()){
            node = (Node*)graphML;
        }
    }
    return node;
}


Edge *NewController::getEdgeFromID(int ID)
{
    GraphML* graphML = getGraphMLFromID(ID);
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



Node *NewController::getNodeFromGraphML(GraphML *item)
{
    Node* node = dynamic_cast<Node*>(item);
    return node;
}

Edge *NewController::getEdgeFromGraphML(GraphML *item)
{
    Edge* edge = dynamic_cast<Edge*>(item);
    return edge;
}

bool NewController::isGraphMLInModel(GraphML *item)
{
    if(model){
        return model->isAncestorOf(item);
    }else{
        return false;
    }
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
    return currentTime.toString("yyyy-MM-dd | hh:mm:ss");
}

Model *NewController::getModel()
{
    return model;
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
 * @brief NewController::setGraphMLData Sets the Value of the GraphMLData of an Entity.
 * @param parentID - The ID of the Entity
 * @param keyName - The name of the Key
 * @param dataValue - The new value of the Data.
 */
void NewController::setGraphMLData(int parentID, QString keyName, QString dataValue)
{
    GraphML* graphML = getGraphMLFromID(parentID);
    if(graphML){
        setGraphMLData(graphML, keyName, dataValue, true);
    }
}

void NewController::setGraphMLData(int parentID, QString keyName, qreal dataValue)
{
    GraphML* graphML = getGraphMLFromID(parentID);
    if(graphML){
        setGraphMLData(graphML, keyName, dataValue, true);
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
                Edge* edge = src->getConnectingEdge(hDst);
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
                Edge* edge =  src->getConnectingEdge(dst);
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
            if(!src || !src->isConnected(dst)){
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
    if(!_importProjects(xmlDataList)){
        emit controller_ActionProgressChanged(100);
    }
    emit controller_ActionFinished();
}

/**
 * @brief NewController::exportProject
 */
void NewController::exportProject()
{
    _exportProject();
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
    emit controller_ActionFinished();
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

bool NewController::askQuestion(MESSAGE_TYPE messageType, QString questionTitle, QString question, int ID)
{
    //Construct a EventLoop which waits for the View to answer the question.
    QEventLoop waitLoop;
    questionAnswer = false;

    if(ID == -1){
        ID = getModel()->getID();
    }
    connect(this, SIGNAL(controller_GotQuestionAnswer()), &waitLoop, SLOT(quit()));

    emit controller_AskQuestion(messageType, questionTitle, question, ID);

    waitLoop.exec();
    return questionAnswer;
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
    //Key Lookup provides a way for the original key "id" to be linked with the internal object GraphMLKey
    QMap<QString , GraphMLKey*> keyLookup;

    //Node lookup provides a way for the original edge source/target ID's to be linked with the internal object Node
    QMap<QString, Node *> nodeLookup;

    //A list for storing the current Parse <data> tags owned by a <node>
    QList<GraphMLData*> currentNodeData;

    //A list storing all the Edge information (source, target, data)
    QList<EdgeTemp> currentEdges;
    GraphMLKey * currentKey;

    //Used to keep track of state inside the XML
    GraphML::KIND nowParsing = GraphML::NONE;
    GraphML::KIND nowInside = GraphML::NONE;

    //Used to store the ID of the node we are to construct
    QString nodeID;

    //Used to store the ID of the graph we are to construct
    QString graphID;

    //Used to store the ID of the new node we will construct later.
    QString newNodeID;

    //If we have been passed no parent, set it as the graph of this Model.
    if(!parent){
        parent = getModel();
    }


    if(parent->isInstance() || parent->isImpl()){
        if(!(UNDOING || REDOING || IMPORTING_SNIPPET)){
            //emit controller_DialogMessage(CRITICAL, "Error", "Cannot import or paste into an Instance/Implementation.", parent);
            emit controller_DisplayMessage(WARNING, "Error", "Cannot import or paste into an Instance/Implementation.", parent->getID());
            return false;
        }
    }

    if(!isGraphMLValid(document)){
        qCritical() << "INVALID DOCUMENT";
        emit controller_DisplayMessage(CRITICAL, "Import Error", "Cannot import; invalid GraphML document.", parent->getID());
        return false;
    }



    Node* originalParent = parent;


    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);

    //Get the number of lines in the input GraphML XML String.
    float lineCount = document.count("\n");

    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;


    //While the document has more lines.
    while(!xml.atEnd()){
        QTime newTimer;
        newTimer.start();
        //Read the next tag
        xml.readNext();


        //Calculate the current percentage
        float lineNumber = xml.lineNumber();
        double percentage = (lineNumber * 100 / lineCount);
        if(!(UNDOING || REDOING)){
            controller_ActionProgressChanged((int)percentage);
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
                nowInside = GraphML::EDGE;
            }
            if(xml.isEndElement()){
                //Set the current object type to NONE.
                nowInside = GraphML::NONE;
            }
        }else if(tagName == "data"){
            if(xml.isStartElement()){
                //Get the datas corresponding Key ID
                QString keyID = getXMLAttribute(xml, "key");

                GraphMLKey* dataKey = keyLookup[keyID];

                if(!dataKey){
                    qCritical() << QString("Line #%1: Cannot find the <key> to match the <data key=\"%2\">").arg(QString::number(xml.lineNumber()), keyID);
                    continue;
                }

                //Get the value of the value of the data tag.
                QString dataValue = xml.readElementText();

                //Construct a GraphMLData object out of the xml, using the key found in keyLookup
                GraphMLData *data = new GraphMLData(dataKey, dataValue);

                //If we aren't linking the ID's we don't need to maintain sortOrder!
                if(!linkID && data->getKeyName() == "sortOrder"){
                    delete data;
                    continue;
                }


                if(resetPos && originalParent == parent){
                    QString keyName = dataKey->getName();
                    if(keyName == "x" || keyName == "y"){
                        data->setValue("-1");
                    }
                }

                //Attach the data to the current object.
                switch(nowInside){
                //Attach the Data to the TempEdge if we are currently inside an Edge.
                case GraphML::EDGE:{
                    currentEdges.last().data.append(data);
                    break;
                }
                    //Attach the Data to the list of Data to be attached to the node.
                case GraphML::NODE:{
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
                nowInside = GraphML::KEY;
                //Parse the Attribute Definition.
                QString name = getXMLAttribute(xml, "attr.name");
                QString typeStr = getXMLAttribute(xml, "attr.type");
                QString forStr = getXMLAttribute(xml, "for");

                currentKey = constructGraphMLKey(name, typeStr, forStr);

                //Get the Key ID.
                QString keyID = getXMLAttribute(xml,"id");

                //Place the key in the lookup Map.
                keyLookup.insert(keyID, currentKey);
            }
            if(xml.isEndElement()){
                nowInside = GraphML::NONE;
            }
        }else if(tagName =="default"){
            if(nowInside == GraphML::KEY){
                QString defaultValue = xml.readElementText();
                currentKey->setDefaultValue(defaultValue);
            }
        }else if(tagName == "node"){
            //If we have found a new <node> it means we should build the previous <node id=nodeID> node.
            if(xml.isStartElement()){
                //Get the ID of the Node
                newNodeID = getXMLAttribute(xml, "id");
                nowInside = GraphML::NODE;
                nowParsing = GraphML::NODE;
            }
            if(xml.isEndElement()){
                //Increase the depth, as we have seen another </node>
                parentDepth++;
                //We have reached the end of a Node, therefore not inside a Node anymore.
                nowInside = GraphML::NONE;
            }
        }else if(tagName == "graph"){
            if(xml.isStartElement()){
                //Get the ID of the Graph, we want to point this at the current Parent.
                graphID = getXMLAttribute(xml, "id");
            }
        }else{
            if(xml.isEndDocument()){
                //Construct the final Node
                nowParsing = GraphML::NODE;
            }else{
                nowParsing = GraphML::NONE;
            }
        }


        if(nowParsing == GraphML::NODE){
            //If we have a nodeID to build
            if(nodeID != ""){
                //Construct the specialised Node
                Node* newNode = constructChildNode(parent, currentNodeData);

                if(!newNode){
                    //emit controller_DialogMessage(CRITICAL, "Import Error", QString("Line #%1: entity cannot adopt child entity!").arg(xml.lineNumber()), parent);
                    qDebug() << QString("Line #%1: entity cannot adopt child entity!").arg(xml.lineNumber());
                    emit controller_DisplayMessage(WARNING, "Paste Error", "Cannot import/paste into this entity.", parent->getID());
                    return false;
                }

                //Clear the Node Data List.
                currentNodeData.clear();

                //Set the currentParent to the Node Construced
                parent = newNode;

                //Navigate back to the correct parent.
                while(parent && parentDepth > 0){
                    parent = parent->getParentNode();
                    parentDepth --;
                }

                //Add the new Node to the lookup table.
                nodeLookup[nodeID] = newNode;

                if(linkID){
                    bool okay;
                    int oldID = nodeID.toInt(&okay);
                    if(okay){
                        linkOldIDToID(oldID, newNode->getID());
                    }
                }

                //If we have encountered a Graph object, we should point it to it's parent Node to allow links to Graph's
                if(graphID != ""){
                    nodeLookup.insert(graphID, newNode);
                    graphID = "";
                }
            }
            //Set the current nodeID to equal the newly found NodeID.
            nodeID = newNodeID;
        }
    }

    if(!(UNDOING || REDOING)){
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
                qCritical() << edge.source;
                qCritical() << edge.target;
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
            if(!s->isConnected(d) && !errorMessage){
                emit controller_DisplayMessage(CRITICAL, "Import Error", "Cannot construct edge!");
                errorMessage = true;
            }
            retry = false;
        }




        Edge* newEdge = constructEdgeWithGraphMLData(s, d, edge.data, getIntFromQString(edge.id));
        if(newEdge){
            sortedEdges.removeFirst();
            edgesMade ++;
        }else{
            sortedEdges.removeFirst();
            if(!s->isConnected(d) && retry){
                retryCount[edge.id] += 1;
                sortedEdges.append(edge);
            }else{
                edgesMade ++;
            }
        }

        if(!(UNDOING || REDOING)){
              emit controller_ActionProgressChanged((100 * totalEdges)/edgesMade);
          }
    }


    if(!(UNDOING || REDOING)){
        controller_ActionProgressChanged(100);
    }

    return true;
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
            if(node->isImpl() && node->getDefinition()){
                if(node->getDataValue("kind") != "OutEventPortImpl"){
                    return false;
                }
            }

            if(node->isInstance() && node->getParentNode()->isInstance()){
                return false;
            }
        }
    }
    return true;
}

bool NewController::canPaste(QList<int> selection)
{
    if(selection.size() == 1){
        GraphML* graphml = getGraphMLFromID(selection[0]);\
        if(graphml && graphml->isNode() && graphml != model){
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
        if(nonSnippetableKinds.contains(node->getDataValue("kind"))){
            return false;
        }
        if(!parent){
            //Set the firstParent to the first Nodes parent.
            parent = node->getParentNode();
            if(!parent){
                return false;
            }
            if(!snippetableParentKinds.contains(parent->getDataValue("kind"))){
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

    if(snippetableParentKinds.contains(parent->getDataValue("kind"))){
        return true;
    }
    return false;
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

bool NewController::areIDsInSameBranch(int mainID, int newID)
{
    GraphML* main = getGraphMLFromID(mainID);
    GraphML* other = getGraphMLFromID(newID);

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

QString NewController::getDataValueFromKeyName(QList<GraphMLData *> dataList, QString keyName)
{
    foreach(GraphMLData* data, dataList){
        if(data->getKeyName() == keyName){
            return data->getValue();
        }
    }
    return "";
}


