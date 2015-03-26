#include "controller.h"
#include "../View/nodeview.h"
#include <QDebug>
#include <algorithm>
#include <QDateTime>
#include <QMessageBox>

#define USE_LOGGING true

bool UNDO = true;
bool REDO = false;
bool SETUP_AS_INSTANCE = true;
bool SETUP_AS_IMPL = false;

NewController::NewController()
{

    logFile = new QFile("output.log");
    if (!logFile->open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text)){
        //CANNOT OPEN LOG FILE.

    }

    UNDOING = false;
    REDOING = false;
    CUT_USED = false;
    model = 0;

    //Construct
    behaviourDefinitions = 0;
    interfaceDefinitions = 0;
    deploymentDefinitions = 0;
    hardwareDefinitions = 0;
    assemblyDefinitions = 0;

    currentActionID = 0;
    actionCount = 0;
    currentAction = "";


    viewAspects << "Assembly" << "Workload" << "Definitions" << "Hardware";
    protectedKeyNames << "x" << "y" << "kind"; //<< "width" << "height";


    containerNodeKinds << "Model";
    containerNodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    containerNodeKinds << "HardwareDefinitions" << "AssemblyDefinitions" << "ManagementComponent";
    containerNodeKinds << "HardwareCluster";

    definitionNodeKinds << "File" << "Component" << "Attribute" << "ComponentAssembly" << "ComponentInstance" ;
    definitionNodeKinds << "Member" << "Aggregate";
    definitionNodeKinds << "InEventPort"  << "OutEventPort";
    definitionNodeKinds << "InEventPortDelegate"  << "OutEventPortDelegate";
    definitionNodeKinds << "AggregateInstance";
    definitionNodeKinds << "ComponentImpl";


    behaviourNodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload";
    behaviourNodeKinds << "OutEventPortImpl";

    //Append Kinds which can't be constructed by the GUI.
    constructableNodeKinds << "MemberInstance" << "AttributeImpl";
    constructableNodeKinds << "OutEventPortInstance" << "MemberInstance" << "AggregateInstance";
    constructableNodeKinds << "AttributeInstance" << "AttributeImpl";
    constructableNodeKinds << "InEventPortInstance" << "InEventPortImpl";
    constructableNodeKinds << "OutEventPortInstance" << "OutEventPortImpl" << "HardwareNode";

    constructableNodeKinds.append(definitionNodeKinds);
    constructableNodeKinds.append(behaviourNodeKinds);
}

void NewController::connectView(NodeView *view)
{
    view->setController(this);


    //Connect SIGNALS to view Slots (ALL VIEWS)
    connect(this, SIGNAL(controller_GraphMLConstructed(GraphML*)), view, SLOT(constructGUIItem(GraphML*)));
    connect(this, SIGNAL(controller_GraphMLDestructed(QString)), view, SLOT(destructGUIItem(QString)));
    connect(this, SIGNAL(controller_ViewSetEnabled(bool)), view, SLOT(setEnabled(bool)));

    if(view->isMainView()){
        //Pass Through Signals to GUI.
        connect(this, SIGNAL(controller_ProjectNameChanged(QString)), view, SIGNAL(view_ProjectNameChanged(QString)));
        connect(this, SIGNAL(controller_ExportedProject(QString)), view, SIGNAL(view_ExportedProject(QString)));
        connect(this, SIGNAL(controller_SetClipboardBuffer(QString)), view, SIGNAL(view_SetClipboardBuffer(QString)));
        connect(this, SIGNAL(controller_StatusChanged(QString)), view, SIGNAL(view_StatusChanged(QString)));
        connect(this, SIGNAL(controller_UndoListChanged(QStringList)), view, SIGNAL(view_UndoListChanged(QStringList)));
        connect(this, SIGNAL(controller_RedoListChanged(QStringList)), view, SIGNAL(view_RedoListChanged(QStringList)));

        //Signals to the View.
        connect(this, SIGNAL(controller_DialogMessage(MESSAGE_TYPE,QString, GraphML*)), view, SLOT(showDialogMessage(MESSAGE_TYPE,QString, GraphML*)));

        // Re-added this for now
        connect(this, SIGNAL(componentInstanceConstructed(Node*)), view, SLOT(componentInstanceConstructed(Node*)));
    }

    if(view->isMainView()){
        connect(view, SIGNAL(view_Duplicate(QStringList)), this, SLOT(duplicateSelection(QStringList)));
        //File SLOTS
        connect(view, SIGNAL(view_ExportProject()), this, SLOT(exportGraphMLDocument()));
        connect(view, SIGNAL(view_ImportProjects(QStringList)), this, SLOT(importProjects(QStringList)));

        //Edit SLOTS
        connect(view, SIGNAL(view_Undo()), this, SLOT(undo()));
        connect(view, SIGNAL(view_Redo()), this, SLOT(redo()));
        connect(view, SIGNAL(view_Delete(QStringList)), this, SLOT(deleteSelection(QStringList)));
        connect(view, SIGNAL(view_Copy(QStringList)), this, SLOT(copy(QStringList)));
        connect(view, SIGNAL(view_Cut(QStringList)), this, SLOT(cut(QStringList)));
        connect(view, SIGNAL(view_Paste(Node*,QString)), this, SLOT(paste(Node*,QString)));

        //Node Slots
        connect(view, SIGNAL(view_ConstructEdge(Node*,Node*)), this, SLOT(constructEdge(Node*,Node*)));
        connect(view, SIGNAL(view_ConstructNode(Node*,QString,QPointF)), this, SLOT(constructNode(Node*,QString,QPointF)));
        connect(view, SIGNAL(view_ConstructComponentInstance(Node*,Node*,QPointF)), this, SLOT(constructComponentInstance(Node*,Node*,QPointF)));
        connect(view, SIGNAL(view_ConstructConnectedComponents(Node*,Node*,QString,QPointF)), this, SLOT(constructConnectedComponents(Node*,Node*,QString,QPointF)));

        //Undo SLOTS
        connect(view, SIGNAL(view_TriggerAction(QString)), this, SLOT(triggerAction(QString)));
        connect(view, SIGNAL(view_SetGraphMLData(GraphML*,QString,QString)), this, SLOT(setGraphMLData(GraphML*,QString,QString)));
    }
}



void NewController::initializeModel()
{
    setupModel();
    clearHistory();
}

NewController::~NewController()
{
    //DELETING = true;
    destructNode(model, false);


    while(keys.size() > 0){
        GraphMLKey* key = keys.takeFirst();
        delete key;
    }

}

QString NewController::_exportGraphMLDocument(QStringList nodeIDs, bool allEdges, bool GUI_USED)
{
    bool exportAllEdges = allEdges;

    QString keyXML, edgeXML, nodeXML;
    QList<Node*> containedNodes;
    QList<GraphMLKey*> containedKeys;
    QList<Edge*> containedEdges;

    //Get all Children and Edges.
    foreach(QString ID, nodeIDs){
        Node* node = getNodeFromID(ID);

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


    foreach(QString ID, nodeIDs){
        Node* node = getNodeFromID(ID);

        foreach(Edge* edge, node->getEdges()){
            Node* src = (Node*) edge->getSource();
            Node* dst = (Node*) edge->getDestination();

            //If the source and destination for all edges are inside the selection, then copy it.
            bool containsSrc = containedNodes.contains(src);
            bool containsDst = containedNodes.contains(dst);
            bool exportEdge = false;
            if(containsSrc && containsDst){
                exportEdge = true;
            }else if(containsSrc || containsDst){
                if((edge->isAggregateLink() || edge->isInstanceLink() || edge->isImplLink())){
                    if(GUI_USED){
                        controller_DialogMessage(MESSAGE, "", src);
                        int reply = QMessageBox::question(0, "Copy Question", "The current selection contains edges not fully encapsulated by selection, would you still like to copy these Edges?", QMessageBox::Yes|QMessageBox::No);
                        if (reply == QMessageBox::Yes) {
                            exportAllEdges = true;
                        }
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
    QStringList nodeIDs;
    nodeIDs.append(node->getID());
    return _exportGraphMLDocument(nodeIDs, allEdges, GUI_USED);
}

QStringList NewController::getNodeKinds(Node *parent)
{
    QStringList empty;
    if(parent){
        if(parent->isImpl() || parent->getParentNode()->isImpl()){
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

QStringList NewController::getConstructableNodeKinds()
{
    return constructableNodeKinds;
}



void NewController::exportGraphMLDocument()
{
    if(model){
        QString data = _exportGraphMLDocument(model);
        controller_ExportedProject(data);
    }
}

void NewController::clearModel()
{
    triggerAction("Clearing Model");

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
        destructNode(child);
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

    clearUndoRedoStacks();
}



void NewController::setGraphMLData(GraphML *parent, QString keyName, QString dataValue, bool addAction)
{
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


    GraphMLData* data = parent->getData(keyName);

    if(data){
        action.dataValue = data->getValue();

        if(keyName == "label"){
            Node* node = dynamic_cast<Node*>(parent);
            enforceUniqueLabel(node, dataValue);

 		}else if(keyName == "sortOrder"){
            Node* node = dynamic_cast<Node*>(parent);
            enforceUniqueSortOrder(node, dataValue.toInt());
            QString newSortOrder = node->getDataValue("sortOrder");

            if(action.dataValue == newSortOrder){
                //Don't add an action for the initial setting!
                addAction = false;
            }

        }else{
            data->setValue(dataValue);
        }

        addActionToStack(action, addAction);
    }else{
        qCritical() << "view_UpdateGraphMLData() Doesn't Contain GraphMLData for Key: " << keyName;
        return;
    }
}

void NewController::attachGraphMLData(GraphML *parent, GraphMLData *data, bool addAction)
{
    //if(DELETING){
    //    return;
    //}
    if(!parent){
        qCritical() << "attachGraphMLData() Parent is NULL!";
        return;
    }

    if(!data){
        qCritical() << "attachGraphMLData() data is NULL!";
        return;
    }

    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::DATA;
    action.keyName = data->getKeyName();


    parent->attachData(data);


    addActionToStack(action, addAction);

}

void NewController::destructGraphMLData(GraphML *parent, QString keyName, bool addAction)
{
    if(!parent){
        qCritical() << "destructGraphMLData() Parent is NULL!";
        return;
    }

    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = DESTRUCTED;
    action.actionKind = GraphML::DATA;
    action.keyName = keyName;

    GraphMLData* data = parent->getData(keyName);

    if(!data){
        qCritical() << "destructGraphMLData(): " << parent->toString() << " doesn't contain Data with Key: " << keyName;
        return;
    }

    action.dataValues.append(data->toStringList());
    action.boundDataIDs.append(data->getBoundIDS());


    if(data->getParentData()){
        QString parentDataID = data->getParentData()->getID();
        action.parentDataID.append(parentDataID);
    }


    addActionToStack(action, addAction);


    parent->removeData(data);
    delete data;
}





void NewController::constructNode(Node *parentNode, QString kind, QPointF centerPoint)
{
    if(kind != ""){
        triggerAction("Constructing Child Node");
        constructChildNode(parentNode, constructGraphMLDataVector(kind, centerPoint));
    }
}

void NewController::constructEdge(Node *src, Node *dst)
{
    constructEdgeWithData(src, dst);
}

Edge* NewController::constructEdgeWithGraphMLData(Node *src, Node *dst, QList<GraphMLData *> data, QString previousID)
{
    Edge* edge = _constructEdge(src, dst);
    if(edge){
        _attachGraphMLData(edge, data, false);

        //If we are Undo-ing or Redo-ing and we have an ID to link it to.
        if((UNDOING || REDOING) && previousID != ""){
            linkOldIDToID(previousID, edge->getID());
        }
        constructEdgeGUI(edge);
    }
    return edge;
}

Edge* NewController::constructEdgeWithData(Node *src, Node *dst, QList<QStringList> attachedData, QString previousID)
{
    Edge* edge = _constructEdge(src, dst);
    if(edge){
        _attachGraphMLData(edge, attachedData, false);

        if((UNDOING || REDOING) && previousID != ""){
            linkOldIDToID(previousID, edge->getID());
        }

        constructEdgeGUI(edge);

    }


    if(!src->isConnected(dst)){
        qCritical() << edge;
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
}

void NewController::clearUndoRedoStacks()
{
    actionCount = 0;
    currentActionID = 0;
    undoActionStack.clear();
    redoActionStack.clear();
}



void NewController::undo()
{
    undoRedo(UNDO);
}

void NewController::redo()
{
    undoRedo(REDO);
}


void NewController::copy(QStringList selectedIDs)
{
    if(!canCopyIDs(selectedIDs)){
        return;
    }

    CUT_USED = false;
    //Export the GraphML for those Nodes.
    QString result = _exportGraphMLDocument(selectedIDs, false, true);

    //Tell the view to place the resulting GraphML String into the Copy buffer.
    controller_SetClipboardBuffer(result);
}

void NewController::cut(QStringList selectedIDs)
{
    //Run Copy
    if(canCopyIDs(selectedIDs)){
        controller_ViewSetEnabled(false);

        triggerAction("Cutting Selected IDs");

        copy(selectedIDs);
        deleteSelection(selectedIDs);

        controller_ViewSetEnabled(true);
        CUT_USED = true;
    }
}

void NewController::deleteSelection(QStringList selectedIDs)
{
    controller_ViewSetEnabled(false);
    triggerAction("Deleting Selection");

    while(!selectedIDs.isEmpty()){
        QString ID = selectedIDs.takeFirst();
        deleteIDs.clear();
        GraphML* graphML = getGraphMLFromID(ID);
        if(graphML){
            if(graphML->isNode()){
                destructNode((Node*)graphML);
            }else if (graphML->isEdge()){
                destructEdge((Edge*)graphML);
            }
        }
        selectedIDs = deleteIDs + selectedIDs;

    }
    controller_ViewSetEnabled(true);
}

void NewController::duplicateSelection(QStringList selectedIDs)
{

    if(!canCopyIDs(selectedIDs)){
        return;
    }

    Node* node = getNodeFromID(selectedIDs.first());
    if(!node){
        return;
    }
    Node* parentNode = node->getParentNode();


    QString result = _exportGraphMLDocument(selectedIDs, true);
    if(parentNode && result != ""){
        _importGraphMLXML(result, parentNode);
    }
}

void NewController::paste(Node *parentNode, QString xmlData)
{

    if(!parentNode){
        controller_DialogMessage(WARNING, "Please Select a Node to Paste Into!");
        return;
    }
    if(isGraphMLValid(xmlData) && xmlData != ""){
        //Paste it into the current Selected Node,

        if(parentNode){
            triggerAction("Pasting Selection.");
            qCritical() << "CUT USED: " << CUT_USED;
            _importGraphMLXML(xmlData, parentNode, CUT_USED);
            CUT_USED = false;
        }
    }
}

/**
     * @brief NewController::constructComponentInstance
     * @param assembly
     * @param definition
     * @param center
     */
void NewController::constructComponentInstance(Node *assembly, Node *definition, QPointF center)
{
    QString instanceKind = getNodeInstanceKind(definition);
    Node* instance = constructChildNode(assembly, constructGraphMLDataVector(instanceKind));

    if(instance){
        //Update the position
        setGraphMLData(instance, "x", QString::number(center.x()));
        setGraphMLData(instance, "y", QString::number(center.y()));

        constructEdge(instance, definition);
    }

    emit componentInstanceConstructed(instance);
}

void NewController::constructConnectedComponents(Node *parent, Node *connectedNode, QString kind, QPointF relativePosition)
{
    Node* newNode = constructChildNode(parent, constructGraphMLDataVector(kind));
    bool gotEdge = false;
    qCritical()<<"constructConnectedComponents";
    if(newNode){
        //Update the position
        setGraphMLData(newNode, "x", QString::number(relativePosition.x()));
        setGraphMLData(newNode, "y", QString::number(relativePosition.y()));

        constructEdge(newNode, connectedNode);

        //Try the alternate connection.
        if(!newNode->isConnected(newNode)){
            constructEdge(connectedNode, newNode);
        }
        gotEdge = newNode->isConnected(newNode);
    }
    if(!gotEdge){
        destructNode(newNode, false);
    }
}



QStringList NewController::getAdoptableNodeKinds(Node *parent)
{
    QStringList adoptableNodeTypes;

    if(parent){
        foreach(QString nodeKind, getNodeKinds(parent)){
            //Construct a Node of the Kind nodeKind.

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

QList<Node *> NewController::getConnectableNodes(Node *src)
{
    QList<Node*> legalNodes;

    foreach (QString ID, nodeIDs) {
        Node* dst = getNodeFromID(ID);
        if(src && dst && dst != src){
            if (src->canConnect(dst)){
                legalNodes.append(dst);
            }
        }
    }
    return legalNodes;
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
        keysValues << "Attribute" << "Member";
        validValues << "Boolean" << "Byte" << "Char" << "WideChar" << "ShortInteger" << "LongInteger" << "LongLongInteger" << "UnsignedShortInteger" << "UnsignedLongInteger" << "UnsignedLongLongInteger" << "FloatNumber" << "DoubleNumber" << "LongDoubleNumber" << "GenericObject" << "GenericValue" << "GenericValueObject" << "String" << "WideString";
        attribute->appendValidValues(validValues, keysValues);
    }

    if(type == "boolean"){
        QStringList validValues;
        validValues << "true" << "false";
        attribute->appendValidValues(validValues);
    }

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
            qCritical() << "Edge: Source: " << source->toString() << " to Destination: " << destination->toString() << " Cannot be created!";
        }
        return 0;
    }
}

void NewController::storeGraphMLInHash(GraphML *item)
{
    QString ID = item->getID();
    if(IDLookupGraphMLHash.contains(ID)){
        qCritical() << "Hash already contains item with ID: " << ID;
    }else{
        IDLookupGraphMLHash[ID] = item;
        if(item->getKind() == GraphML::NODE){
            nodeIDs.append(ID);
        }else if(item->getKind() == GraphML::EDGE){
            edgeIDs.append(ID);
        }
        controller_GraphMLConstructed(item);
    }
}

GraphML *NewController::getGraphMLFromHash(QString ID)
{
    if(IDLookupGraphMLHash.contains(ID)){
        return IDLookupGraphMLHash[ID];
    }else{
        //qCritical() << "Cannot find GraphML from Lookup Hash. ID: " << ID;
    }
    return 0;
}

void NewController::removeGraphMLFromHash(QString ID)
{
    if(IDLookupGraphMLHash.contains(ID)){
        //qCritical() << "Hash Removed ID: " << ID;
        GraphML* item = IDLookupGraphMLHash[ID];
        if(item)
        {
            controller_GraphMLDestructed(ID);
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


    Node* node = constructTypedNode(childNodeKind, childNodeType);

    //Enforce Default Data!
    QList<GraphMLData*> requiredData = constructGraphMLDataVector(childNodeKind);

    if(node){

        //Attach/Update GraphMLData
        bool undoState = isGraphMLInModel(node);
        //Attach Default Data.
        _attachGraphMLData(node, requiredData, undoState);
        //Update Data with custom Data!
        _attachGraphMLData(node, nodeData, undoState);
    }

    //Delete the non-adopted data.
    while(!nodeData.isEmpty()){
        GraphMLData* data = nodeData.takeFirst();
        if(!node || !node->containsData(data)){
            delete data;
        }
    }
    while(!requiredData.isEmpty()){
        GraphMLData* data = requiredData.takeFirst();
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


    if(!isGraphMLInModel(node)){
        if(parentNode->canAdoptChild(node)){
            parentNode->addChild(node);

            //Force Unique labels and Sort Order. Can only happen after adoption.
            enforceUniqueLabel(node);
            enforceUniqueSortOrder(node);

            constructNodeGUI(node);
        }else{
            delete node;
            return 0;
        }
    }

    //If the node is a definition, construct an instance/Implementation in each Instance/Implementation of the parentNode.
    if(node && node->isDefinition()){
        foreach(Node* child, parentNode->getInstances()){
            constructDefinitionRelative(child, node, true);
        }
        foreach(Node* child, parentNode->getImplementations()){
            constructDefinitionRelative(child, node, false);
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



    QList<GraphMLData*> data;

    QString labelString = nodeKind;


    data.append(new GraphMLData(kindKey, nodeKind));
    data.append(new GraphMLData(xKey, QString::number(relativePosition.x())));
    data.append(new GraphMLData(yKey, QString::number(relativePosition.y())));
    data.append(new GraphMLData(widthKey, "0"));
    data.append(new GraphMLData(heightKey, "0"));
    data.append(new GraphMLData(labelKey, labelString));
    data.append(new GraphMLData(sortKey, "-1"));


    //Attach Node Specific Data.

    if(nodeKind == "ManagementComponent"){      
        data.append(new GraphMLData(typeKey, ""));

    }
    if(nodeKind == "Model"){
        GraphMLKey* middlewareKey = constructGraphMLKey("middleware", "string", "node");
        GraphMLKey* projectUUID = constructGraphMLKey("projectUUID", "string", "node");

        data.append(new GraphMLData(projectUUID, getTimeStamp()));
        data.append(new GraphMLData(middlewareKey, "tao"));
    }
    if(nodeKind == "PeriodicEvent"){
        GraphMLKey* frequencyKey = constructGraphMLKey("frequency", "double", "node");
        data.append(new GraphMLData(typeKey, "Constant"));
        data.append(new GraphMLData(frequencyKey, "1.0"));
    }
    if(nodeKind == "Process"){
        GraphMLKey* codeKey = constructGraphMLKey("code", "string", "node");
        GraphMLKey* actionOnKey = constructGraphMLKey("actionOn", "string", "node");
        data.append(new GraphMLData(codeKey, ""));
        data.append(new GraphMLData(actionOnKey, "Mainprocess"));
    }
    if(nodeKind == "Member"){
        GraphMLKey* keyKey = constructGraphMLKey("key", "boolean", "node");
        data.append(new GraphMLData(keyKey, "false"));
        data.append(new GraphMLData(typeKey, "String"));
    }
    if(nodeKind == "MemberInstance"){
        GraphMLKey* keyKey = constructGraphMLKey("key", "boolean", "node");
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        data.append(new GraphMLData(valueKey, ""));
        data.append(new GraphMLData(keyKey, "false"));
    }
    if(nodeKind == "Attribute"){
        data.append(new GraphMLData(typeKey, "String"));
    }
    if(nodeKind == "AttributeInstance"){
        GraphMLKey* valueKey = constructGraphMLKey("value", "string", "node");
        data.append(new GraphMLData(valueKey, ""));
    }

    if(nodeKind == "OutEventPortInstance" || nodeKind == "InEventPortInstance"){
        GraphMLKey* topicKey = constructGraphMLKey("topicName", "string", "node");
        data.append(new GraphMLData(topicKey, ""));
    }

    if(nodeKind.endsWith("Instance") ||nodeKind.endsWith("Impl")){
        data.append(new GraphMLData(typeKey, ""));
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
    if(definitionKind == "Attribute" || definitionKind.startsWith("Member")){
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
            if(requiresLabelAndType && labelMatched || !requiresLabelAndType){
                Edge* connectingEdge = constructEdgeWithData(child, definition);

                if(!connectingEdge){
                    qCritical() << "constructDefinitionRelative(): Couldn't construct Edge between Relative Node and Definition Node.";
                    continue;
                }
                //Don't create an ActionItem for this.
                connectingEdge->setGenerated(true);
                nodesMatched++;
            }
        }
    }

    //If we didn't find a match, we must create an Instance.
    if(nodesMatched == 0){
        Node *instanceNode = constructChildNode(parent, constructGraphMLDataVector(childKind));

        if(!instanceNode){
            qCritical() << "constructDefinitionRelative(): Couldn't construct a Relative Node.";
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
    if(!node){
        return;
    }

    Node* parentNode = node->getParentNode();

    if(nodeLabel == ""){
        nodeLabel = node->getDataValue("label");
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


    for(labelID; labelID <= maxNumber; labelID++){
        if(!sameLabelNumbers.contains(labelID)){
            break;
        }
    }

    if(labelID > 0){
        nodeLabel += QString("_%1").arg(labelID);
    }


     node->updateDataValue("label", nodeLabel);
}

void NewController::enforceUniqueSortOrder(Node *node, int newPosition)
{
    if(!node){
        return;
    }

    Node* parentNode = node->getParentNode();

    if(parentNode){
        bool toInt;
        int maxSortOrder = parentNode->childrenCount() -1;

        int originalPosition = node->getDataValue("sortOrder").toInt(&toInt);

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

        bool isInt;
        foreach(Node* sibling, node->getSiblings()){
            int currentPos = sibling->getDataValue("sortOrder").toInt(&isInt);
            if(isInt && currentPos >= lowerPos && currentPos <= upperPos){
                sibling->updateDataValue("sortOrder", QString::number(currentPos + modifier));
            }
        }
    }
    node->updateDataValue("sortOrder", QString::number(newPosition));
}

bool NewController::destructNode(Node *node, bool addAction)
{
    if(!node){
        qCritical() << "NewController::destructNode() Got NULL Node!";
        return true;
    }
    if(addAction){
        if(behaviourDefinitions == node){
            qCritical() << "Cannot delete behaviourDefinitions. Must be deleted from Definition.";
            return false;
            //behaviourDefinitions = 0;
        }
        if(deploymentDefinitions == node){
            qCritical() << "Cannot delete deploymentDefinitions. Must be deleted from Definition.";
            return false;
            //deploymentDefinitions = 0;
        }
        if(interfaceDefinitions == node){
            qCritical() << "Cannot delete interfaceDefinitions. Must be deleted from Definition.";
            return false;
            //interfaceDefinitions = 0;
        }
        if(assemblyDefinitions == node){
            qCritical() << "Cannot delete assemblyDefinitions. Must be deleted from Definition.";
            return false;
            //assemblyDefinitions = 0;
        }
        if(hardwareDefinitions == node){
            qCritical() << "Cannot delete hardwareDefinitions. Must be deleted from Definition.";
            return false;
        }
    }

    //Gotta Delete in Order.
    QString XMLDump = "";
    QString ID = node->getID();



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
        Edge* edge = node->getEdges(0).first();
        if(edge){
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

    if(!node->wasGenerated()){
        //Add an action to reverse this action.
        ActionItem action;
        action.actionKind = GraphML::NODE;
        action.actionType = DESTRUCTED;
        action.removedXML = XMLDump;
        action.ID = ID;
        if(node->getParentNode()){
            action.parentID = node->getParentNode()->getID();
        }
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

    delete node;

    return true;
}










bool NewController::destructEdge(Edge *edge, bool addAction)
{
    if(!edge){
        qCritical() << "destructEdge(): Edge is NULL";
        return true;
    }


    //Get information about the edge.
    QString ID = edge->getID();
    Node* source = edge->getSource();
    Node* destination = edge->getDestination();

    if(!(source && destination)){
        qCritical() << "destructEdge(): Source and/or Destination are NULL.";
        return false;
    }

    //If the Edge Wasn't Generated, and we are meant to add an Action for this removal, Add an undo state.
    if(!edge->wasGenerated()){
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
        addActionToStack(action, addAction);
    }

    //Node* toDeleteNode = 0;
    //Delete the Instan
    if(edge->isInstanceLink()){
        //If Edge represents an Instance relationship; Tear it down.
        bool success = teardownDefinitionRelationship(destination, source, SETUP_AS_INSTANCE);
    }else if(edge->isImplLink()){
        //If Edge represents an Implementation relationship; Tear it down.
        bool success = teardownDefinitionRelationship(destination, source, SETUP_AS_IMPL);
    }else if(edge->isAggregateLink()){
        //If Edge represents an Implementation relationship; Tear it down.
        EventPort* eventPort = dynamic_cast<EventPort*>(source);
        Aggregate* aggregate = dynamic_cast<Aggregate*>(destination);
        bool success = teardownAggregateRelationship(eventPort, aggregate);
    }else if(edge->isComponentLink()){
        // UnBind Topics Together.
        GraphMLData* sourceTopicName = source->getData("topicName");
        GraphMLData* destinationTopicName = destination->getData("topicName");
        if(destinationTopicName && sourceTopicName ){
            sourceTopicName->unbindData(destinationTopicName);
        }
    }

    //Remove it from the hash of GraphML
    removeGraphMLFromHash(ID);

    //Delete it.
    delete edge;

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

void NewController::reverseAction(ActionItem action)
{
    //Switch on the Action Type.
    switch(action.actionType){
    case CONSTRUCTED:{

        switch(action.actionKind){
        case GraphML::NODE:{
            //Delete Node.
            Node* node = getNodeFromID(action.ID);
            if(node){

                destructNode(node);

            }
            break;
        }
        case GraphML::EDGE:{
            //Delete Edge.
            Edge* edge = getEdgeFromID(action.ID);
            if(edge){
                destructEdge(edge, true);
            }
            break;
        }
        case GraphML::DATA:{
            //Delete Data
            GraphML* item = getGraphMLFromID(action.ID);

            if(item){
                destructGraphMLData(item, action.keyName);
            }else{
                qCritical() << "\ncase CONSTRUCTED:GraphML::DATA Cannot find Item";
                qCritical() << "action.ID: " << action.ID;
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
            //Get Parent Node, and Construct Node.
            Node* parentNode = getNodeFromID(action.parentID);
            if(parentNode){
                _importGraphMLXML(action.removedXML, parentNode, true);
            }else{
                qCritical() << "Cannot find Node";
            }
            break;
        }
        case GraphML::EDGE:{
            //Get Source and Destination, attempt to construct an Edge.

            Node* src = getNodeFromID(action.srcID);
            Node* dst = getNodeFromID(action.dstID);


            if(src && dst){
                if(isEdgeLegal(src,dst)){
                    constructEdgeWithData(src ,dst, action.dataValues, action.ID);
                }else{
                    if(!src->isConnected(dst)){
                        qCritical() << "Cannot make Edge: " << src->toString() << " AND  " << dst->toString();
                    }
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
                //Restore the Data Value;
                setGraphMLData(attachedItem, action.keyName, action.dataValue);
            }else{
                //if(!IS_SUB_VIEW){
                qCritical() << action.ID << " " << action.keyName <<  action.dataValue;
                qCritical() << "Cannot find Item";
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

    if(useAction){
        if(UNDOING){
            redoActionStack.push(action);
        }else{
            undoActionStack.push(action);
        }
        updateViewUndoRedoLists();
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

        controller_ViewSetEnabled(false);


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
        //qCritical() << "Actions to Reverse: " << actionCount;
        for(int i = 0; i < actionCount; i++){
            int percentage = (i*100) / actionCount;
            controller_ActionProgressChanged(percentage, "Undoing");
            reverseAction(toReverse[i]);
        }

        controller_ActionProgressChanged(100);


        if(UNDOING){
            undoActionStack = actionStack;
        }else{
            redoActionStack = actionStack;
        }


        updateViewUndoRedoLists();


        controller_ViewSetEnabled(true);

        UNDOING = false;
        REDOING = false;
}

void NewController::logAction(ActionItem item)
{

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


    out << item.timestamp << "\t" << actionType << "\t" << actionKind << "\t" << item.ID;
    if(extraInfo != ""){
        out << extraInfo;
    }
    out << "\n";



}

void NewController::clearHistory()
{
    currentActionID = 0;
    actionCount = 0;
    currentAction = "";
    undoActionStack.clear();
    redoActionStack.clear();

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
        }
        return new HardwareNode();
    }else if(nodeKind == "HardwareCluster"){
        return new HardwareCluster();
    }else if(nodeKind == "ManagementComponent"){
        if(managementComponents.contains(nodeType)){
            return managementComponents[nodeType];
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
    }else if(nodeKind == "File"){
        return new File();
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
    }else if(nodeKind == "Termination"){
        return new Termination();
    }else if(nodeKind == "Variable"){
        return new Variable();
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

    //Add Action to the Undo/Redo Stack.
    addActionToStack(action);

    //Add the node to the list of nodes constructed.
    storeGraphMLInHash(node);
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


    setupManagementComponents();
	//Clear the Undo/Redo Stacks
    undoActionStack.clear();
    redoActionStack.clear();
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

    bool bindTypes = true;
    bool bindLabels = false;
    bool bindSort = false;

    if(!definition->getDefinition() && !def_Type){
        bindTypes = false;
    }
    if((child->isInstance() || child->isImpl()) || (def_Type && def_Label)){
        if(child->getDataValue("kind") == "AggregateInstance" || child->getDataValue("kind") == "MemberInstance"){
            bindSort = true;
        }
        if(child->getDataValue("kind") != "ComponentInstance"){
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
            //Construct relationships between the children which matched the definitionChild.
            int instancesConnected = constructDefinitionRelative(node, child, instance);

            if(instancesConnected == 0){
                qCritical() << "setupDefinitionRelationship(): Couldn't create a Definition Relative for: " << child->toString() << " In: " << node->toString();
                return false;
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
        constructEdge(aggregateInstance, aggregate);
        edge = aggregateInstance->getConnectingEdge(aggregate);
        //Set it is AutoGenerated as to not make an undo state for it.
        edge->setGenerated(true);
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

    QList<Node*> aggregateInstances = eventPort->getChildrenOfKind("AggregateInstance", 0);
    if(aggregateInstances.size() == 1){
        Node* child = aggregateInstances[0];
        if(child){
            //Add the AggregateInstance to the list of Nodes to delete.
            deleteIDs.append(child->getID());
        }else{
            return false;
        }
    }else{
        return false;
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


    deleteIDs.append(node->getID());

    return true;
}

bool NewController::isGraphMLValid(QString inputGraphML)
{
    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xmlErrorChecking(inputGraphML);

    //Check for Errors
    while(!xmlErrorChecking.atEnd()){
        xmlErrorChecking.readNext();
        float lineNumber = xmlErrorChecking.lineNumber();
        if (xmlErrorChecking.hasError()){
            //qCritical() << "isGraphMLValid(): Parsing Error! Line #" << lineNumber;
            //qCritical() << "\t" << xmlErrorChecking.errorString();
            //qCritical() << inputGraphML;
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
            Aggregate* aggregate = dynamic_cast<Aggregate*>(dst);
            setupAggregateRelationship(eventPort, aggregate);
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

    storeGraphMLInHash(edge);
}


void NewController::updateViewUndoRedoLists()
{
    QList<int> actions;
    QStringList undoList;
    QStringList redoList;

    //Undo first
    foreach(ActionItem a, undoActionStack){
        int ID = a.actionID;
        if(actions.contains(ID) == false){
            actions.append(ID);
            undoList.insert(0, a.actionName);
        }
    }
    actions.clear();

    //Redo
    foreach(ActionItem a, redoActionStack){
        int ID = a.actionID;
        if(actions.contains(ID) == false){
            actions.append(ID);
            redoList.insert(0, a.actionName);
        }
    }

    controller_UndoListChanged(undoList);
    controller_RedoListChanged(redoList);
}

void NewController::setupManagementComponents()
{

    GraphMLKey* typeKey = constructGraphMLKey("type", "string", "node");

    //EXECUTION MANAGER
    QList<GraphMLData*> executionManagerData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> dancePlanLauncherData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> ddsLoggingServerData = constructGraphMLDataVector("ManagementComponent") ;

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


    Node* emNode = constructChildNode(assemblyDefinitions, executionManagerData);
    Node* plNode = constructChildNode(assemblyDefinitions, dancePlanLauncherData);
    Node* lsdNode = constructChildNode(assemblyDefinitions, ddsLoggingServerData);


    managementComponents.insert("DANCE_EXECUTION_MANAGER",dynamic_cast<ManagementComponent*>(emNode));
    managementComponents.insert("DANCE_PLAN_LAUNCHER",dynamic_cast<ManagementComponent*>(plNode));
    managementComponents.insert("DDS_LOGGING_SERVER",dynamic_cast<ManagementComponent*>(lsdNode));
}

GraphML *NewController::getGraphMLFromID(QString ID)
{
    //Check for old IDs
    ID = getIDFromOldID(ID);

    GraphML* graphML = getGraphMLFromHash(ID);
    return graphML;
}

Node *NewController::getNodeFromID(QString ID)
{
    GraphML* graphML = getGraphMLFromID(ID);
    return getNodeFromGraphML(graphML);
}


Edge *NewController::getEdgeFromID(QString ID)
{
    GraphML* graphML = getGraphMLFromID(ID);
    return getEdgeFromGraphML(graphML);
}

QString NewController::getIDFromOldID(QString ID)
{
    QString originalID = ID;
    QString newID = ID;
    while(newID != ""){
        if(IDLookupHash.contains(ID)){
            QString temp = ID;
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


void NewController::linkOldIDToID(QString oldID, QString newID)
{
    //Point the ID Hash for the oldID to the newID
    IDLookupHash[oldID] = newID;
    if(!IDLookupHash.contains(newID)){
        //Set the ID Hash for the newID to ""
        IDLookupHash[newID] = "";
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

QString NewController::getTimeStamp()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyyy-MM-dd | hh:mm:ss");
}

Model *NewController::getModel()
{
    return model;
}

void NewController::importProjects(QStringList documents)
{
    controller_ViewSetEnabled(false);
    triggerAction("Importing Documents");

    foreach(QString document, documents){
        bool result = _importGraphMLXML(document, getModel());
        if(!result){
            controller_DialogMessage(WARNING, "Cannot Import Document!", getModel());
        }
    }

    controller_ViewSetEnabled(true);
}

void NewController::clearUndoHistory()
{
    clearHistory();
}

bool NewController::_importGraphMLXML(QString document, Node *parent, bool linkID)
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
        if(!(UNDOING || REDOING)){
            emit controller_DialogMessage(CRITICAL, "Cannot Import or Paste into a Instance/Implementation", parent);
            return false;
        }
    }

    if(!isGraphMLValid(document)){
        emit controller_DialogMessage(CRITICAL, "Cannot Import; Invalid GraphML document.", parent);
        return false;
    }

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);

    //Get the number of lines in the input GraphML XML String.
    float lineCount = document.count("\n");

    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;

    //While the document has more lines.
    while(!xml.atEnd()){
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

                //Check to see if we parsed that key already.
                if(!keyLookup.contains(keyID)){
                    qCritical() << QString("Line #%1: Cannot find the <key> to match the <data key=\"%2\">").arg(QString::number(xml.lineNumber()),keyID);
                    break;
                }

                //Get the value of the value of the data tag.
                QString dataValue = xml.readElementText();

                //Construct a GraphMLData object out of the xml, using the key found in keyLookup
                GraphMLData *data = new GraphMLData(keyLookup[keyID], dataValue);

                //If we aren't linking the ID's we don't need to maintain sortOrder!
                if(!linkID && data->getKeyName() == "sortOrder"){
                    delete data;
                    continue;
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
                    emit controller_DialogMessage(CRITICAL, QString("Import Error; Line #%1: Node Cannot Adopt child Node!").arg(xml.lineNumber()), parent);
                    break;
                }

                //Clear the Node Data List.
                currentNodeData.clear();

                //Set the currentParent to the Node Construced
                parent = newNode;

                //Navigate back to the correct parent.
                while(parentDepth > 0){
                    parent = parent->getParentNode();
                    parentDepth --;
                }

                //Add the new Node to the lookup table.
                nodeLookup.insert(nodeID, newNode);

                if(linkID){
                    linkOldIDToID(nodeID, newNode->getID());
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

    //Sort the edges into types.
    QList<EdgeTemp> aggregateEdges;
    QList<EdgeTemp> instanceEdges;
    QList<EdgeTemp> implEdges;
    QList<EdgeTemp> otherEdges;

    foreach(EdgeTemp edge, currentEdges){
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];

        if(!s){
            s = getNodeFromID(edge.source);
        }
        if(!d){
            d = getNodeFromID(edge.target);
        }

        if(!(s && d)){
            emit controller_DialogMessage(CRITICAL, QString("Import Error: Got Invalid Edge!"));
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

    while(sortedEdges.size() > 0){
        EdgeTemp edge = sortedEdges.first();

        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];

        if(!s){
            s = getNodeFromID(edge.source);
        }
        if(!d){
            d = getNodeFromID(edge.target);
        }


        bool retry = true;
        if(retryCount[edge.id] > maxRetry){
            emit controller_DialogMessage(CRITICAL, QString("Import Error: Cannot Construct Edge!"));
            if(!s->isConnected(d)){
                qCritical() << "Cannot Created Edge:" << edge.id;
            }
            retry = false;
        }




        Edge* newEdge = constructEdgeWithGraphMLData(s, d, edge.data, edge.id);
        if(newEdge){
            sortedEdges.removeFirst();
        }else{
            sortedEdges.removeFirst();
            if(!s->isConnected(d) && retry){
                retryCount[edge.id] += 1;
                sortedEdges.append(edge);
            }
        }
    }

   if(!(UNDOING || REDOING)){
       controller_ActionProgressChanged(100);
   }

   return true;
}

bool NewController::canCopyIDs(QStringList IDs)
{
    Node* parent = 0;
    if(IDs.length() == 0){
        return false;
    }

    foreach(QString ID, IDs){
        Node* node = getNodeFromID(ID);

        if(!node){
            //Probably an Edge!
            continue;
        }
        if(!parent){
            //Set the firstParent to the first Nodes parent.
            parent = node->getParentNode();
        }

        if(node->getParentNode() != parent){
            controller_DialogMessage(WARNING, "Can only Cut or Copy Nodes which share the same Parent.", node);
            return false;
        }
    }
    return true;
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
