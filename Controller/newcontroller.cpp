#include "newcontroller.h"
#include "../GUI/nodeview.h"
#include <QDebug>

#include <algorithm>

bool UNDO = true;
bool REDO = false;
bool SETUP_AS_INSTANCE = true;
bool SETUP_AS_IMPL = false;

NewController::NewController()
{
    UNDOING = false;
    REDOING = false;
    CUT_USED = false;
    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;
    HIDDEN_OPACITY = 0.10;

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


    centeredGraphML = 0;

    viewAspects << "Assembly" << "Workload" << "Definitions" << "Hardware";
    protectedKeyNames << "x" << "y" << "kind"; //<< "width" << "height";


    containerNodeKinds << "Model";
    containerNodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    containerNodeKinds << "HardwareDefinitions" << "AssemblyDefinitions";

    constructableNodeKinds << "MemberInstance" << "AggregateInstance";
    constructableNodeKinds << "File" << "Component" << "ComponentInstance" << "ComponentImpl";
    constructableNodeKinds << "Attribute" << "AttributeInstance" << "AttributeImpl";
    constructableNodeKinds << "InEventPort" << "InEventPortInstance" << "InEventPortImpl" << "InEventPortDelegate";
    constructableNodeKinds << "OutEventPort" << "OutEventPortInstance" << "OutEventPortImpl" << "OutEventPortDelegate";
    constructableNodeKinds << "ComponentAssembly" << "ManagementComponent";
    constructableNodeKinds << "HardwareNode" << "HardwareCluster" ;
    constructableNodeKinds << "Member" << "Aggregate";
    constructableNodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload";

}

void NewController::connectView(NodeView *view)
{
    view->setController(this);

    //Connect to the View's Signals
    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftPressed(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeletePressed(bool)));
    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(unselect()),this, SLOT(view_ClearSelection()));
    connect(view, SIGNAL(unselect()), this, SLOT(view_UncenterGraphML()));
    connect(view, SIGNAL(constructNodeItem(QString, QPointF)), this,SLOT(view_ConstructNode(QString, QPointF)));
    connect(view, SIGNAL(escapePressed(bool)), this, SLOT(view_ClearSelection()));

    connect(view, SIGNAL(sortModel()), this, SLOT(view_SortModel()));
    connect(view, SIGNAL(centerModel()), this, SLOT(centerModel()) );
    connect(this, SIGNAL(centerModel(Node*)), view, SLOT(centreModel(Node*)) );

    //Connect the View to the Controllers Signals
    connect(this, SIGNAL(view_SetOpacity(GraphML*,qreal)), view, SLOT(view_SetOpacity(GraphML*,qreal)));
    connect(this, SIGNAL(view_SortNode(Node*)), view, SLOT(view_SortNode(Node*)));
    connect(this, SIGNAL(view_SetGraphMLSelected(GraphML*,bool)), view, SLOT(view_SelectGraphML(GraphML*,bool)));
    //connect(this, SIGNAL(view_ForceRefresh()), view, SLOT(view_Refresh()));
    connect(this, SIGNAL(view_CenterGraphML(GraphML*)), view, SLOT(view_CenterGraphML(GraphML*)));
    connect(this, SIGNAL(view_SetRubberbandSelectionMode(bool)), view, SLOT(setRubberBandMode(bool)));
    connect(this, SIGNAL(view_ConstructGraphMLGUI(GraphML*)), view, SLOT(view_ConstructGraphMLGUI(GraphML*)));
    connect(this, SIGNAL(view_DestructGraphMLGUIFromID(QString)), view, SLOT(view_DestructGraphMLGUI(QString)));
    connect(this, SIGNAL(view_PrintErrorCode(GraphML*,QString)), view, SLOT(printErrorText(GraphML*,QString)));

    connect(this, SIGNAL(view_FitToScreen()), view, SLOT(fitToScreen()));
}


void NewController::initializeModel()
{
    setupModel();
    setupValidator();
}

NewController::~NewController()
{
    view_ClearSelection();
    destructNode(model, false);

    while(keys.size() > 0){
        GraphMLKey* key = keys.takeFirst();
        delete key;
    }
    delete validator;
}

QString NewController::exportGraphML(QStringList nodeIDs)
{
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
        if(!node){
            qCritical() << "Dead Node";
        }
        foreach(Edge* edge, node->getEdges()){
            Node* src = (Node*) edge->getSource();
            Node* dst = (Node*) edge->getDestination();

            //If the source and destination for all edges are inside the selection, then copy it.
            if(containedNodes.contains(src) && containedNodes.contains(dst)){
                if(containedEdges.contains(edge) == false){
                    containedEdges.append(edge);
                    edgeXML += edge->toGraphML(2);
                }
            }
            //Get the Keys related to this edge.
            foreach(GraphMLKey* key, edge->getKeys()){
                if(!containedKeys.contains(key)){
                    containedKeys.append(key);
                    keyXML += key->toGraphML(1);
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

QString NewController::exportGraphML(Node *node)
{
    QStringList nodes;
    nodes.append(node->getID());
    return exportGraphML(nodes);
}

QStringList NewController::getNodeKinds()
{
    return constructableNodeKinds;
}

QStringList NewController::getViewAspects()
{
    return viewAspects;
}

QStringList NewController::getNodeIDS()
{
    return nodeIDs;
}

void NewController::view_ImportGraphML(QStringList inputGraphML, Node *currentParent)
{
    view_TriggerAction("Importing GraphML");
    emit view_SetGUIEnabled(false);
   // emit view_EnableGUI(false);
   // emit view_UpdateProgressDialog(true);

    int files = inputGraphML.size();

    for (int i = 0; i < files ; i++){
        //Update Dialogs etc.
       // emit view_UpdateProgressDialog(0,QString("Importing GraphML file %1 / %2").arg(QString::number(i + 1), QString::number(files)));

        QString currentGraphMLData = inputGraphML.at(i);
        //emit controller_ActionTrigger("Importing GraphML");
        view_ImportGraphML(currentGraphMLData, currentParent);
    }

     emit view_SetGUIEnabled(true);
    // emit view_UpdateProgressDialog(false);

}

void NewController::view_ImportGraphML(QString inputGraphML, Node *currentParent, bool linkID)
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
    QString nodeID = "";

    //Used to store the ID of the graph we are to construct
    QString graphID = "";

    //Used to store the ID of the new node we will construct later.
    QString newNodeID = "";

    //If we have been passed no parent, set it as the graph of this Model.
    if(currentParent == 0){
        currentParent = getModel();
    }

    if(currentParent->isInstance() || currentParent->isImpl()){
        if(!(UNDOING || REDOING)){
            qCritical() << "Cannot Import or Copy and Paste into a Instace/Impl";
            return;
        }
    }


    if(!isGraphMLValid(inputGraphML)){
        qCritical() << "GraphML is not valid!";
        return;
    }


    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(inputGraphML);

    //Get the number of lines in the input GraphML XML String.
    float lineCount = inputGraphML.count("\n");


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
            emit view_UpdateProgressBar((int)percentage);
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
                    delete(data);
                }
            }
            if(xml.isEndElement()){
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
                Node* newNode = constructChildNode(currentParent, currentNodeData);

                //Delete unadopted Memory
                while(currentNodeData.size()> 0){
                    GraphMLData* data = currentNodeData.takeFirst();
                    if(!newNode || !newNode->containsData(data)){
                        //Delete newly Constructed unadopted data.
                        delete data;
                    }
                }

                if(newNode == 0){
                     qCritical() << QString("Line #%1: Node Cannot Adopt child Node!").arg(xml.lineNumber());
                     break;
                }

                //Clear the Node Data List.
                currentNodeData.clear();

                //Set the currentParent to the Node Construced
                currentParent = newNode;


                //Navigate back to the correct parent.
                while(parentDepth > 0){
                    currentParent = currentParent->getParentNode();
                    //Check if the parent is a graph or a node!
                    //We only care about if it's not a graph!
                    //Graph* graph = dynamic_cast<Graph*> (currentParent);
                   // if(graph != 0){
                    parentDepth --;
                    //}
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
        if(!s || !d){
            qCritical() << "Got Broken Edge";
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
        bool retry = true;
        if(retryCount[edge.id] > maxRetry){
            retry = false;
        }
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];
        Edge* newEdge = constructEdgeWithData(s, d, edge.data, edge.id);
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

    /*
    foreach(EdgeTemp edge, sortedEdges){
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];
        constructEdgeWithData(s, d, edge.data, edge.id);
    }
    */

    if(!(UNDOING || REDOING)){
        emit view_UpdateProgressBar(100);
    }
}

void NewController::view_ClearModel()
{
    //qCritical() << "Clearing Model";
    //qCritical() << actionCount;
    view_TriggerAction("Clearing Model");

    //qCritical() << actionCount;

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

    view_ClearUndoRedoStacks();
}

void NewController::validator_HighlightError(Node *node, QString error)
{
    if(node){
        emit view_CenterGraphML(node);
        emit view_PrintErrorCode(node, error);
        emit view_UpdateStatusText(error);
    }
}

void NewController::view_ValidateModel()
{
    if(validator && model){
        validator->validateModel(model);
    }
}

void NewController::view_UpdateGraphMLData(GraphML *parent, QString keyName, QString dataValue)
{
    //Construct an Action to reverse the update
    ActionItem action;
    action.ID = parent->getID();
    action.actionType = MODIFIED;
    action.actionKind = GraphML::DATA;
    action.keyName = keyName;

    //Update
    GraphMLData* data = parent->getData(keyName);

    if(data){
        action.dataValue = data->getValue();
        addActionToStack(action);
        data->setValue(dataValue);
    }else{
        qCritical() << "Data Doesn't Exist";
    }
}

void NewController::view_ConstructGraphMLData(GraphML *parent, QString keyName)
{
    Q_UNUSED(parent);
    //TODO: Implement
}

void NewController::view_DestructGraphMLData(GraphML *parent, QString keyName)
{
    if(parent){
        //Construct an Action to reverse the update
        ActionItem action;
        action.ID = parent->getID();
        action.actionType = DESTRUCTED;
        action.actionKind = GraphML::DATA;
        action.keyName = keyName;

        GraphMLData* data = parent->getData(keyName);

        if(data){
            action.dataValues.append(data->toStringList());
            action.boundDataIDs.append(data->getBoundIDS());

           QString parentDataID = "";
           if(data->getParentData()){
               parentDataID = data->getParentData()->getID();
               action.parentDataID.append(parentDataID);
           }

           //qCritical() << "Removed Data: " << keyName << " from ID: " << action.ID;
           addActionToStack(action);

           //Remove!
           parent->removeData(data);
           delete data;
        }else{
            qCritical() << "action.ID: " << action.ID;
            qCritical() << "keyName: " << keyName;
            qCritical() << "Data Doesn't Exist";
        }
    }else{
        qCritical() << "Parent Doesn't Exist";
    }
}

void NewController::view_CenterComponentImpl(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && node->getImplementations().size() == 1){
        emit view_CenterGraphML(node->getImplementations()[0]);
        //emit view_CenterGraphML(node->getImplementations()[0]);
    }
}

void NewController::view_CenterComponentDefinition(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && node->getDefinition()){

        emit view_CenterGraphML(node->getDefinition());
    }

}

void NewController::view_CenterAggregate(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && (node->getDefinition() || node->isDefinition())){
        if(!node->isDefinition()){
            node = node->getDefinition();
        }


        EventPort* eP = dynamic_cast<EventPort*>(node);
        if(eP && eP->getAggregate()){
             emit view_CenterGraphML(eP->getAggregate());
        }
    }
}

void NewController::view_ProjectNameUpdated(GraphMLData *label)
{
    emit view_UpdateProjectName(label->getValue());
}


void NewController::view_ExportGraphML(QString filename)
{
    QString data = exportGraphML(model);
    emit view_WriteGraphML(filename, data);
}

void NewController::view_GraphMLSelected(GraphML *item, bool setSelected)
{
    Node* node = getNodeFromGraphML(item);
    Edge* edge = getEdgeFromGraphML(item);

    if(node){
        if(KEY_CONTROL_DOWN){
            //If Node is already selected, Unselect It.
            setSelected = !isNodeSelected(node);
            setNodeSelected(node, setSelected);
            return;
        }

        if(KEY_SHIFT_DOWN){
            //Construct an Edge between the previous selected Node and this Node.
            Node* source = getSelectedNode();
            if(!getSelectedEdge() && source){
                view_TriggerAction("Constructing Edge.");

                view_ConstructEdge(source, node);

                return;
            }
        }

        if(!isNodeSelected(node)){
            //Clear Selected Items
            view_ClearSelection();
            //Set the Node Selected
            setNodeSelected(node, setSelected);
        }
    }

    if(edge){
        if(KEY_CONTROL_DOWN){
            //If Edge is already selected, Unselect It.
            setSelected = !isEdgeSelected(edge);
            setEdgeSelected(edge, setSelected);
            return;
        }

        if(!isEdgeSelected(edge)){
            //Clear Selected Items
            view_ClearSelection();
            //Set the Edge Selected
            setEdgeSelected(edge, setSelected);
        }
    }
}

void NewController::view_ConstructNode(QString kind, QPointF centerPoint)
{
    if(kind != ""){
        view_TriggerAction("Constructing Child Node");
        constructChildNode(getSelectedNode(), constructGraphMLDataVector(kind, centerPoint));
    }
}

void NewController::view_ConstructComponentInstanceInAssembly(Component *definition, ComponentAssembly *assembly)
{

    if(assembly == 0){
        foreach(Node* child, assemblyDefinitions->getChildren(0)){
            ComponentAssembly* childAssembly = dynamic_cast<ComponentAssembly*>(child);
            if(childAssembly){
                assembly = childAssembly;
                break;
            }
        }
    }
    if(definition && assembly){
        view_TriggerAction("Constructing Component Instance");

        Node* instanceNode = constructChildNode(assembly, getDefinitionData(definition, true));

        instanceNode->getData("x")->setValue("0");
        instanceNode->getData("y")->setValue("0");
        /*if(instanceNode){
            //Don't create an ActionItem for this.
            instanceNode->setGenerated(true);
        }
        */

        if(instanceNode){
            QList<QStringList> noData;
            Edge* connectingEdge = constructEdgeWithData(instanceNode, definition, noData);
            /*if(connectingEdge){
                //Don't create an ActionItem for this.
                connectingEdge->setGenerated(true);
            }
            */
        }

    }

}


void NewController::view_ConstructEdge(Node *src, Node *dst)
{
    QList<QStringList> noData;

    constructEdgeWithData(src, dst, noData);
}

Edge* NewController::constructEdgeWithData(Node *src, Node *dst, QList<GraphMLData *> data, QString previousID)
{
    Edge* edge = constructEdge(src, dst);
    if(edge){
        attachGraphMLData(edge, data, false);

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
    Edge* edge = constructEdge(src, dst);
    if(edge){
        attachGraphMLData(edge, attachedData, false);

        if((UNDOING || REDOING) && previousID != ""){
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

void NewController::view_MoveSelectedNodes(QPointF delta)
{
    foreach(QString ID, selectedNodeIDs){
        Node* node = getNodeFromID(ID);
        if(node){
            GraphMLData* xData = node->getData("x");
            GraphMLData* yData = node->getData("y");

            float x = xData->getValue().toFloat() + delta.x();
            float y = yData->getValue().toFloat() + delta.y();

            view_UpdateGraphMLData(node, "x", QString::number(x));
            view_UpdateGraphMLData(node, "y", QString::number(y));
        }
    }

}

void NewController::view_SelectFromID(QString ID)
{
    GraphML* item = getGraphMLFromID(ID);
    if(item){

        view_ClearSelection();
        view_GraphMLSelected(item, true);
        view_CenterGraphML(item);
    }
}

void NewController::view_FilterNodes(QStringList filterString)
{
    foreach(QString ID, nodeIDs){
        Node* node = getNodeFromID(ID);
            bool allMatched = true;
            foreach(QString filter, filterString){
                bool gotMatch = false;
                foreach(GraphMLData* data, node->getData()){
                    if(data->getValue().contains(filter)){
                        gotMatch = true;
                        break;
                    }
                }
                if(!gotMatch){
                    allMatched = false;
                    break;
                }
            }
            if(allMatched){
                emit view_SetOpacity(node, 1);
            }else{
                emit view_SetOpacity(node, 0);
            }


    }
}

void NewController::view_SortModel()
{
    qDebug() << "NewController: sortModel";
    emit view_SortNode(model);
}

void NewController::view_ShowLegalEdgesForNode(Node *src)
{
    emit view_ForceRefresh();
    foreach(QString ID, nodeIDs){
        Node* dst = getNodeFromID(ID);

        if(dst && !src->canConnect(dst) && (dst != src)){
            emit view_SetOpacity(dst, HIDDEN_OPACITY);
        }
    }
    emit view_ForceRefresh();
}

void NewController::view_ShowAllNodes()
{
    foreach(QString ID, nodeIDs){
        Node* node = getNodeFromID(ID);
        if(node){
            emit view_SetOpacity(node, 1);
        }
    }
}

void NewController::view_TriggerAction(QString actionName)
{
    actionCount++;
    currentAction = actionName;
    currentActionID = actionCount;
}

void NewController::view_ClearUndoRedoStacks()
{
    undoActionStack.clear();
    redoActionStack.clear();
}

void NewController::view_ControlPressed(bool isDown)
{
    KEY_CONTROL_DOWN = isDown;
    if(KEY_CONTROL_DOWN && KEY_SHIFT_DOWN){
        emit view_SetRubberbandSelectionMode(true);
    }else{
        emit view_SetRubberbandSelectionMode(false);

    }
}

void NewController::view_ShiftPressed(bool isDown)
{
    KEY_SHIFT_DOWN = isDown;

    if(KEY_CONTROL_DOWN && KEY_SHIFT_DOWN){
        emit view_SetRubberbandSelectionMode(true);
    }else{
        emit view_SetRubberbandSelectionMode(false);
    }

    if(KEY_SHIFT_DOWN){
        Node* node = getSelectedNode();
        if(node){
            view_ShowLegalEdgesForNode(node);
        }
    }else{
        view_ShowAllNodes();

    }

}

void NewController::view_DeletePressed(bool isDown)
{
     if(isDown){
         emit view_SetGUIEnabled(false);
         view_TriggerAction("Deleting Selection");
         deleteSelectedEdges();
         deleteSelectedNodes();
         emit view_SetGUIEnabled(true);
     }

     // this removes the non-disappearing
     // blue lines after deletion
     emit view_ForceRefresh();
}

void NewController::view_Undo()
{
    undoRedo(UNDO);
}

void NewController::view_Redo()
{
    undoRedo(REDO);
}

void NewController::view_Copy()
{
    copySelectedNodesGraphML();
}

void NewController::view_Cut()
{
    //Run Copy
    if(copySelectedNodesGraphML()){
        CUT_USED = true;
        emit view_SetGUIEnabled(false);
        view_TriggerAction("Cutting Selection.");
        deleteSelectedEdges();
        deleteSelectedNodes();
        emit view_SetGUIEnabled(true);
    }
}

void NewController::view_Paste(QString xmlData)
{

    if(isGraphMLValid(xmlData) && xmlData != ""){
        //Paste it into the current Selected Node,
        Node* node = getSelectedNode();

        //Paste into the current Maximized Node.

        Node* centeredNode = getNodeFromGraphML(centeredGraphML);
        if(centeredNode){
            node = centeredNode;
        }

        //Clear Selection.
        clearSelectedEdges();
        clearSelectedNodes();

        view_TriggerAction("Pasting Selection.");
        view_ImportGraphML(xmlData, node, CUT_USED);
        CUT_USED = false;
    }
}

void NewController::view_ClearKeyModifiers()
{
}

void NewController::view_SelectAll()
{

    clearSelectedEdges();
    clearSelectedNodes();

    Node* centeredNode = getNodeFromGraphML(centeredGraphML);
    if(centeredNode){
        //Get children.

        foreach(Node* child, centeredNode->getChildren(2)){
            Node* node = dynamic_cast<Node*>(child);
            if(node){
                setNodeSelected(node);
            }
        }
    }else{
        //Get children.
        QList<Node*> containers;
        containers << behaviourDefinitions << interfaceDefinitions << hardwareDefinitions << assemblyDefinitions;

        foreach(Node* container, containers){
            foreach(Node* child, container->getChildren(0)){
                setNodeSelected(child);
            }
        }
    }

}

void NewController::view_UncenterGraphML()
{
    centeredGraphML = 0;
}

void NewController::view_ClearSelection()
{
    clearSelectedEdges();
    clearSelectedNodes();
}

bool NewController::copySelectedNodesGraphML()
{
    Node* firstParent = 0;

    qCritical() << "Copying: " << selectedNodeIDs.size() << " Nodes";
    foreach(QString ID, selectedNodeIDs){
        Node* node = getNodeFromID(ID);

        if(!firstParent){
            //Set the firstParent to the first Nodes parent.
            firstParent = node->getParentNode();
        }

        if(node->getParentNode() != firstParent){
            emit view_DialogMessage(WARNING, "Can only copy Nodes which share the same Node Parent");
            return false;
        }
    }

    //Export the GraphML for those Nodes.
    QString result = exportGraphML(selectedNodeIDs);

    //Tell the view to place the resulting GraphML String into the Copy buffer.
    emit view_UpdateCopyBuffer(result);
    return true;
}

QStringList NewController::getAdoptableNodeKinds(Node *parent)
{
    QStringList adoptableNodeTypes;

    if(!parent){
        parent = this->getSelectedNode();
    }

    if(parent){
        foreach(QString nodeKind, getNodeKinds()){
            //Construct a Node of the Kind nodeKind.
            Node* node = 0;
            node = constructNode(constructGraphMLDataVector(nodeKind), true);

            //If we have constructed this node, test if the parent can adopt it.
            if(node){
               if(parent->canAdoptChild(node)){
                   adoptableNodeTypes.append(nodeKind);
               }
            }

            //Delete the node, if we didn't create a GUI NodeItem.
            if(node && !isGraphMLInModel(node)){
                delete node;
            }

        }
    }

    //qCritical() << adoptableNodeTypes;

    return adoptableNodeTypes;
}

void NewController::view_SelectModel()
{
    if(model){
        emit view_SetGraphMLSelected(model);
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

    //Add it to the list of GraphMLKeys.
    keys.append(attribute);
    return attribute;
}


Node *NewController::constructNode(QList<GraphMLData *> dataToAttach, bool readOnly)
{
    //The to-be-constructed node.
    Node* node = 0;

    //Construct/Get Keys for the data we require.
    GraphMLKey* widthKey = constructGraphMLKey("width", "double", "node");
    GraphMLKey* heightKey = constructGraphMLKey("height", "double", "node");
    GraphMLKey* xKey = constructGraphMLKey("x", "double", "node");
    GraphMLKey* yKey = constructGraphMLKey("y", "double", "node");
    GraphMLKey* kindKey = constructGraphMLKey("kind", "string", "node");
    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "node");

    bool setWidth = true;
    bool setHeight = true;
    bool setX = true;
    bool setY = true;
    bool setLabel = true;

    QString typeKind = "";
    QString nodeKind = "";
    QString labelKind = "";

    foreach(GraphMLData* data, dataToAttach){
        if(data->getKeyName() == "kind"){
            nodeKind = data->getValue();
        }else if(data->getKeyName() == "type"){
            typeKind = data->getValue();
        }else if(data->getKeyName() == "label"){
            labelKind = data->getValue();
        }
    }

     QList<GraphMLData*> requiredData = constructGraphMLDataVector(nodeKind);

     while(requiredData.size() > 0){
         GraphMLData* data = requiredData.takeFirst();

         bool gotDataType = false;
         foreach(GraphMLData* gotData, dataToAttach){
             if(gotData->getKey() == data->getKey()){
                 gotDataType = true;
                 break;
             }
         }

         if(!gotDataType){
             dataToAttach.append(data);
         }else{
             delete data;
         }
     }


    //Check Vector of data for X,Y, Width and Height.
    //Also get nodeKind
    foreach(GraphMLData* data, dataToAttach){
        GraphMLKey* dataKey = data->getKey();

        if(dataKey == kindKey){
            //Get the kind of this Node
            nodeKind = data->getValue();
        }else if(dataKey == widthKey){
            setWidth = false;
        }else if(dataKey == heightKey){
            setHeight = false;
        }else if(dataKey == xKey){
            setX = false;
        }else if(dataKey == yKey){
            setY = false;
        }else if(dataKey == labelKey){
            setLabel = false;
        }
    }

    //GET DEFAULT DATA.



    if(nodeKind == ""){
        qCritical() << "Cannot Construct a Kind-less Node.";
        return 0;
    }

    //Attach blank data to the Vector for the Unset Data Keys.
    if(setWidth){
        dataToAttach.append(new GraphMLData(widthKey, ""));
    }
    if(setHeight){
        dataToAttach.append(new GraphMLData(heightKey, ""));
    }

    if(nodeKind != "Model"){
        if(setX){
            dataToAttach.append(new GraphMLData(xKey, ""));
        }
        if(setY){
            dataToAttach.append(new GraphMLData(yKey, ""));
        }
        if(setLabel){
            dataToAttach.append(new GraphMLData(labelKey, ""));
        }
    }

    //Construct Containers First.
    if(nodeKind == "Model"){
        //Check for an existing Model node.
        if(!model){
            model = new Model();
        }
        node = model;
    }else if(nodeKind == "BehaviourDefinitions"){
        if(!behaviourDefinitions){
            behaviourDefinitions = new BehaviourDefinitions();
        }
        node = behaviourDefinitions;
    }else if(nodeKind == "InterfaceDefinitions"){
        if(!interfaceDefinitions){
            interfaceDefinitions = new InterfaceDefinitions();
        }
        node = interfaceDefinitions;
    }else if(nodeKind == "AssemblyDefinitions"){
        if(!assemblyDefinitions){
            assemblyDefinitions = new AssemblyDefinitions();
        }
        node = assemblyDefinitions;
    }else if(nodeKind == "HardwareDefinitions"){
        if(!hardwareDefinitions){
            hardwareDefinitions = new HardwareDefinitions();
        }
        node = hardwareDefinitions;
    }else if(nodeKind == "DeploymentDefinitions"){
        if(!deploymentDefinitions){
            deploymentDefinitions = new DeploymentDefinitions();
        }
        node = deploymentDefinitions;
    }else if(nodeKind == "HardwareNode"){
        if(!readOnly){
            if(!hardwareNodes.contains(labelKind)){
                hardwareNodes[labelKind] = new HardwareNode();
            }
            node = hardwareNodes[labelKind];
        }
    }
    else if(nodeKind == "HardwareCluster"){
        if(!readOnly){
            if(!hardwareClusters.contains(labelKind)){
                hardwareClusters[labelKind] = new HardwareCluster();
            }
            node = hardwareClusters[labelKind];
        }
    }else if(nodeKind == "ManagementComponent"){
        if(!readOnly){
            if(!managementComponents.contains(typeKind)){
                managementComponents[typeKind] = new ManagementComponent();
            }
            node = managementComponents[typeKind];
        }
    }
    else if(nodeKind == "ComponentAssembly"){
        node = new ComponentAssembly();
    }else if(nodeKind == "Component"){
        node = new Component();
    }else if(nodeKind == "ComponentInstance"){
        node = new ComponentInstance();
    }else if(nodeKind == "ComponentImpl"){
        node = new ComponentImpl();
    }else if(nodeKind == "OutEventPort"){
        node = new OutEventPort();
    }else if(nodeKind == "OutEventPortInstance"){
        node = new OutEventPortInstance();
    }else if(nodeKind == "OutEventPortImpl"){
        node = new OutEventPortImpl();
    }else if(nodeKind == "OutEventPortDelegate"){
        node = new OutEventPortDelegate();
    }else if(nodeKind == "InEventPort"){
        node = new InEventPort();
    }else if(nodeKind == "InEventPortInstance"){
        node = new InEventPortInstance();
    }else if(nodeKind == "InEventPortImpl"){
        node = new InEventPortImpl();
    }else if(nodeKind == "InEventPortDelegate"){
        node = new InEventPortDelegate();
    }else if(nodeKind == "Attribute"){
        node = new Attribute();
    }else if(nodeKind == "AttributeInstance"){
        node = new AttributeInstance();
    }else if(nodeKind == "AttributeImpl"){
        node = new AttributeImpl();
    }else if(nodeKind == "File"){
        node = new File();
    }else if(nodeKind == "Member"){
        node = new Member();
    }else if(nodeKind == "Aggregate"){
        node = new Aggregate();
    }else if(nodeKind == "AggregateInstance"){
        node = new AggregateInstance();
    }else if(nodeKind == "MemberInstance"){
        node = new MemberInstance();
    }else if(nodeKind == "BranchState"){
        node = new BranchState();
    }else if(nodeKind == "Condition"){
        node = new Condition();
    }else if(nodeKind == "PeriodicEvent"){
        node = new PeriodicEvent();
    }else if(nodeKind == "Workload"){
        node = new Workload();
    }else if(nodeKind == "Process"){
        node = new Process();
    }else if(nodeKind == "Termination"){
        node = new Termination();
    }else if(nodeKind == "Variable"){
        node = new Variable();
    }else{
        qCritical() << "Node Kind:" << nodeKind << " not yet implemented!";
        node = new BlankNode();
    }


    if((!node || isGraphMLInModel(node)) && readOnly){
        return 0;
    }

    //Adds the data to the newly created Node.
    //This will add Undo states to reverse.
    attachGraphMLData(node, dataToAttach, false);



    //node->addAspect("Definitions");
    return node;
}

Edge *NewController::constructEdge(Node *source, Node *destination)
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

Node *NewController::constructChildNode(Node *parentNode, QList<GraphMLData *> dataToAttach)
{
    //Construct the Model Node first.

    Node* node = constructNode(dataToAttach);

    //Check if this node has been already setup Visually.
    if(isGraphMLInModel(node)){
        //Clean up dataToAttach
        return node;
    }

    //If the node wasn't constructed, return 0
    if(!node){
        qCritical() << "Node was not successfully constructed!";
        return 0;
    }

    //If we have no parentNode, attempt to attach it to the Model.
    if(!parentNode){
        parentNode = model;
    }

    //Check to see if the parentNode defined can adopt this new node.
    if(parentNode->canAdoptChild(node)){
        //Adopt the Node.
        parentNode->addChild(node);
        //Setup the GUI.
        constructNodeGUI(node);
    }else{
        qCritical() << "Node: " << parentNode->toString() << " Cannot Adopt: " << node->toString();
        //Delete the newly created node.

        while(dataToAttach.size() > 0){
            GraphMLData* data = dataToAttach.takeFirst();
            delete data;
        }
        delete node;
        node = 0;
    }

    //If the node is a defintion, construct an instance/Implementation in each Instance/Implementation of the parentNode.
    if(node && node->isDefinition()){
        foreach(Node* child, parentNode->getInstances()){
            constructDefinitionRelative(child, node);
        }
        foreach(Node* child, parentNode->getImplementations()){
            constructDefinitionRelative(child, node, false);
        }
    }

    return node;
}

QList<GraphMLData *> NewController::constructGraphMLDataVector(QString nodeKind, QPointF relativePosition)
{
    QList<GraphMLData*> data;

    GraphMLKey* kindKey = constructGraphMLKey("kind", "string", "node");
    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "node");
    GraphMLKey* xKey = constructGraphMLKey("x", "double", "node");
    GraphMLKey* yKey = constructGraphMLKey("y", "double", "node");

    data.append(new GraphMLData(kindKey, nodeKind));
    data.append(new GraphMLData(xKey, QString::number(relativePosition.x())));
    data.append(new GraphMLData(yKey, QString::number(relativePosition.y())));
    data.append(new GraphMLData(labelKey, nodeKind));

    //Attach Model Specific stuff.
    if(nodeKind == "Attribute"){
        GraphMLKey* typeKey = constructGraphMLKey("type", "string", "node");
        data.append(new GraphMLData(typeKey, ""));
    }
    if(nodeKind == "ManagementComponent"){
        GraphMLKey* typeKey = constructGraphMLKey("type", "string", "node");
        data.append(new GraphMLData(typeKey, ""));
    }
    if(nodeKind == "Model"){
        GraphMLKey* middlewareKey = constructGraphMLKey("middleware", "string", "node");
        GraphMLKey* projectUUID = constructGraphMLKey("projectUUID", "string", "node");

        //Update Later
        data.append(new GraphMLData(projectUUID, "0"));
        data.append(new GraphMLData(middlewareKey, "tao"));
    }
    if(nodeKind == "Member"){
        GraphMLKey* keyKey = constructGraphMLKey("key", "boolean", "node");
        data.append(new GraphMLData(keyKey, "false"));
    }
    if(nodeKind == "OutEventPortInstance" || nodeKind == "InEventPortInstance"){
        GraphMLKey* topicKey = constructGraphMLKey("topicName", "string", "node");
        data.append(new GraphMLData(topicKey, ""));
    }
    if(nodeKind == "Component"){
        GraphMLKey* componentUUIDKey = constructGraphMLKey("componentUUID", "string", "node");
        GraphMLKey* implUUIDKey = constructGraphMLKey("implUUID", "string", "node");
        GraphMLKey* svntUUIDKey = constructGraphMLKey("svntUUID", "string", "node");
        data.append(new GraphMLData(componentUUIDKey, ""));
        data.append(new GraphMLData(implUUIDKey, ""));
        data.append(new GraphMLData(svntUUIDKey, ""));
    }
    if(nodeKind == "ComponentInstance"){
        GraphMLKey* componentInstanceUUIDKey = constructGraphMLKey("componentInstanceUUID", "string", "node");
        data.append(new GraphMLData(componentInstanceUUIDKey, ""));
    }


    return data;
}

QList<GraphMLData *> NewController::constructGraphMLDataVector(QString nodeKind)
{
    //No Position provided, so call with a Blank Point.
    return constructGraphMLDataVector(nodeKind, QPointF(0,0));
}
Node *NewController::constructDefinitionRelative(Node *parent, Node *definition, bool isInstance)
{
    Node* instanceNode = 0;
    QString defLabel = "";
    QString defType = "";
    bool isAttributeInstance = false;

    //If we are dealing with an Attribute, they have a Type.
    if(dynamic_cast<Attribute*>(definition) != 0){
        isAttributeInstance = true;
    }

    //Get the Definitions Label
    if(definition->getData("label")){
        defLabel = definition->getDataValue("label");
    }

    //Get the Definitions Type
    if(definition->getData("type")){
        defType = definition->getDataValue("type");
    }

    //For each child in parent, check to see if any Nodes match Label/Type
    foreach(Node* child, parent->getChildren(0)){
        QString childLabel = "";
        QString childType = "";

        if(child->getDefinition() == definition){
            //If the child already has definition set.
            return child;
        }
        if(child->getDefinition()){
            //If the child has a different definition, move onto the next child.
            continue;
        }

        //Get the Label
        if(child->getData("label")){
            childLabel = child->getDataValue("label");
        }

        //Get the Type
        if(child->getData("type")){
            childType = child->getDataValue("type");
        }

        bool typeMatched = false;
        bool labelMatched = false;

        if(childType != "" && (childType == defType || childType == defLabel)){
            typeMatched = true;
        }

        if(typeMatched && childLabel != "" && (childLabel == defLabel)){
            labelMatched = true;
        }


        if(isAttributeInstance){
            //If we are dealing with an Attribute. Label and Type must match.
            if(typeMatched && labelMatched){
                //Got Match, break loop.
                instanceNode = child;
                break;
            }
        }else{
            //If we aren't dealing with an Attribute, Label or Type must match.
            if(typeMatched || labelMatched){
                //Got Match, break loop.
                instanceNode = child;
                break;
            }
        }
    }

    //If we didn't find a match, we must create an Instance.
    if(!instanceNode){
        //Construct an Instance with the data from the Definition
        instanceNode = constructChildNode(parent, getDefinitionData(definition, isInstance));
        if(instanceNode){
            //Don't create an ActionItem for this.
            instanceNode->setGenerated(true);
        }
    }

    if(instanceNode){
        QList<QStringList> noData;
        Edge* connectingEdge = constructEdgeWithData(instanceNode, definition, noData);
        if(connectingEdge){
            //Don't create an ActionItem for this.
            connectingEdge->setGenerated(true);
        }
    }

    return instanceNode;
}



QList<GraphMLData *> NewController::getDefinitionData(Node *definition, bool forInstance)
{
    QList<GraphMLData *> childData;

    QStringList requiredKeyNames;
    requiredKeyNames << "kind" << "label" << "type" << "x" << "y";

    foreach(GraphMLData* data, definition->getData()){
        GraphMLKey* key = data->getKey();
        QString keyName = key->getName();
        QString keyValue = data->getValue();

        if(requiredKeyNames.contains(keyName)){
            QString modifier = "";

            if(keyName == "kind"){
                //AggregateInstance and MemberInstances are able to Instance themselves.
                if(keyValue != "AggregateInstance" && keyValue != "MemberInstance"){
                    if(forInstance){
                        modifier = "Instance";
                    }else{
                        modifier = "Impl";
                    }
                }
            }
            GraphMLData* newData = new GraphMLData(key, keyValue + modifier);
            //newData->setProtected(true);
            childData.append(newData);
        }
    }

    return childData;
}

void NewController::clearSelectedNodes()
{
    foreach(QString ID, selectedNodeIDs){
        Node* node = getNodeFromID(ID);
        if(node){
            emit view_SetGraphMLSelected(node, false);
        }
    }
    selectedNodeIDs.clear();
    emit view_SetGraphMLSelected(0);
    //emit view_SetGraphMLSelected(0);
}

void NewController::clearSelectedEdges()
{
     foreach(QString ID, selectedEdgeIDs){
         Edge* edge = getEdgeFromID(ID);
         if(edge){
             emit view_SetGraphMLSelected(edge, false);
         }
    }
    selectedEdgeIDs.clear();
}

void NewController::setNodeSelected(Node *node, bool setSelected)
{
    if(!isGraphMLInModel(node)){
        qCritical() << "Node isn't connected to Model.";
        return;
    }

    if(setSelected){
        //Check to see if Node's Parents are in the list of selected Nodes.
        if(!isNodesAncestorSelected(node)){
            //Unselect any children nodes which are contained by the new node.
            foreach(QString ID, selectedNodeIDs){
                Node* selectedNode = getNodeFromID(ID);

                if(node && node->isAncestorOf(selectedNode)){
                    setNodeSelected(selectedNode, false);
                }
            }

            emit view_SetGraphMLSelected(node);

            selectedNodeIDs.append(node->getID());
        }

        //Check all selected Edges.
        foreach(QString ID, selectedEdgeIDs){
            Edge* selectedEdge = getEdgeFromID(ID);
            if(selectedEdge){
                Node* source = selectedEdge->getSource();
                Node* destination = selectedEdge->getDestination();

                //Deselect any edges which are connected to the node selected.
                if(node == source || node == destination){
                    setEdgeSelected(selectedEdge, false);
                }
            }
        }
    }else{
        emit view_SetGraphMLSelected(node, false);

        //Remove Node from Selection List.
        selectedNodeIDs.removeAll(node->getID());
    }
}

void NewController::setEdgeSelected(Edge *edge, bool setSelected)
{
    //TODO: Implement Control Multiple Selection.

    if(!isGraphMLInModel(edge)){
        qCritical() << "Edge isn't connected to Model.";
        return;
    }

    if(setSelected){
        Node* source = edge->getSource();
        Node* destination = edge->getDestination();

        //If Either ends of the Edge's Nodes are not selected.
        if(!isNodeSelected(source) && !isNodeSelected(destination)){
            if(edge->isInstanceLink()){
                //Select all Children Instance Links between src and dst.
                foreach(Edge* childEdge, destination->getEdges()){
                    if(childEdge->isInstanceLink()){
                        if(source->isAncestorOf(childEdge->getSource())){
                            emit view_SetGraphMLSelected(childEdge, true);
                            selectedEdgeIDs.append(childEdge->getID());
                        }
                    }
                }
            }

            emit view_SetGraphMLSelected(edge, true);
            selectedEdgeIDs.append(edge->getID());
        }
    }else{
        emit view_SetGraphMLSelected(edge, false);
        selectedEdgeIDs.removeAll(edge->getID());
    }
}

Node *NewController::getSelectedNode()
{
    if(selectedNodeIDs.size() == 1){
        return getNodeFromID(selectedNodeIDs[0]);
    }
    return 0;
}


/**
 * @brief NewController::centerModel
 */
void NewController::centerModel()
{
    emit centerModel(model);
}

Edge *NewController::getSelectedEdge()
{
    if(selectedEdgeIDs.size() == 1){
        return getEdgeFromID(selectedEdgeIDs[0]);
    }
    return 0;
}

bool NewController::destructNode(Node *node, bool addAction)
{
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
            //hardwareDefinitions = 0;
        }
    }

    //Gotta Delete in Order.
    QString XMLDump = "";
    QString ID = node->getID();
    //qCritical() << "destructNode2(" << ID << ", " << addAction << " )";

    if(addAction){
        //Export only if we are add this node to reverse state.
        XMLDump = exportGraphML(node);
    }

    foreach(Edge* edge, node->getEdges()){
        if(edge){
            //If this node contains both Edge ends, then it will be contained by the XML exported.

            if(node->isAncestorOf(edge->getDestination())){
                destructEdge(edge, true);
            }else{
                destructEdge(edge, false);
            }
        }
    }

    foreach(Node* childNode, node->getChildren(0)){
        QString childID = childNode->getID();
        destructNode(childNode, false);
    }

    if(!node->wasGenerated() && addAction){
        //Add an action to reverse this action.
        ActionItem action;
        action.actionKind = GraphML::NODE;
        action.actionType = DESTRUCTED;
        action.removedXML = XMLDump;
        action.ID = ID;
        if(node->getParentNode()){
            action.parentID = node->getParentNode()->getID();
        }

        addActionToStack(action);
    }

    selectedNodeIDs.removeAll(ID);

    emit view_DestructGraphMLGUIFromID(ID);
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
        QString type = mCNode->getDataValue("type");
        managementComponents.remove(type);
    }

    delete node;

    return true;
}










bool NewController::destructEdge(Edge *edge, bool addAction)
{
    if(edge){
        QString ID = edge->getID();
        if(addAction){
            ActionItem action;
            action.actionType = DESTRUCTED;
            action.actionKind = GraphML::EDGE;
            action.ID = ID;

            action.srcID = edge->getSource()->getID();
            action.dstID = edge->getDestination()->getID();

            foreach(GraphMLData* data, edge->getData()){
                action.dataValues.append(data->toStringList());
            }

            addActionToStack(action);
        }

        Node* source = edge->getSource();
        Node* destination = edge->getDestination();

        if(edge->isInstanceLink()){
            teardpwmDefinitionRelationship(destination, source, SETUP_AS_INSTANCE);
        }else if(edge->isImplLink()){
            teardpwmDefinitionRelationship(destination, source, SETUP_AS_IMPL);
        }else if(edge->isAggregateLink()){
            EventPort* eventPort = dynamic_cast<EventPort*>(source);
            Aggregate* aggregate = dynamic_cast<Aggregate*>(destination);
            if(eventPort && aggregate){
                eventPort->unsetAggregate();
            }
            foreach(Node* implementation, eventPort->getImplementations()){
                foreach(Node* child, implementation->getChildren(0)){
                    if(child->isConnected(aggregate)){
                        child->unsetDefinition();
                    }
                }
            }
        }else if(edge->isDeploymentLink()){
            InEventPortInstance* iEPI = dynamic_cast<InEventPortInstance*>(destination);
            OutEventPortInstance* oEPI = dynamic_cast<OutEventPortInstance*>(source);
            if(iEPI && oEPI){
                //Bind Topics Together.
                GraphMLData* destinationTopicName = iEPI->getData("topicName");
                GraphMLData* sourceTopicName = oEPI->getData("topicName");
                if(destinationTopicName && sourceTopicName ){
                    destinationTopicName->unbindData(sourceTopicName);
                    sourceTopicName->setProtected(false);
                }
            }
        }

        selectedEdgeIDs.removeAll(ID);

        emit view_DestructGraphMLGUIFromID(ID);

        removeGraphMLFromHash(ID);
        delete edge;
        return true;
    }else{
         qCritical() << "Edge doesn't exist!!";
         return false;

    }
}

bool NewController::isNodesAncestorSelected(Node *selectedNode)
{

    foreach(QString ID, selectedNodeIDs){
        Node* node = getNodeFromID(ID);
        if(node && node->isAncestorOf(selectedNode)){
            return true;
        }
    }
    return false;
}

bool NewController::isNodeSelected(Node *node)
{
    QString ID = node->getID();
    return selectedNodeIDs.contains(ID);
}

bool NewController::isEdgeSelected(Edge *edge)
{
    QString ID = edge->getID();
    return selectedEdgeIDs.contains(ID);
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
                view_DestructGraphMLData(item, action.keyName);
            }else{
                qCritical() << "case CONSTRUCTED:GraphML::DATA Cannot find Item";
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
                view_ImportGraphML(action.removedXML, parentNode, true);
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
                bool success = attachGraphMLData(attachedItem, action.dataValues);
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
                view_UpdateGraphMLData(attachedItem, action.keyName, action.dataValue);
                //GraphMLData* data = attachedItem->getData(action.keyName);
                //data->setValue(action.dataValue);
            }else{
                qCritical() << action.ID << " " << action.keyName <<  action.dataValue;
                qCritical() << "Cannot find Item";
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
bool NewController::attachGraphMLData(GraphML *item, QList<QStringList> dataList, bool addAction)
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

    return attachGraphMLData(item, graphMLDataList, addAction);
}

bool NewController::attachGraphMLData(GraphML *item, QList<GraphMLData *> dataList, bool addAction)
{
    if(!item){
        qCritical() << "Null GraphML Item.";
        return false;
    }

    if(getEdgeFromGraphML(item)){
      //  qCritical() << "EDGE DATA" << item->getID();
    }


    foreach(GraphMLData* data, dataList){
        GraphMLData* containedData = item->getData(data->getKey());
        if(containedData){
            //Update so we have an Undo.
            //qWarning() << item->toString() << " Found duplicate Data for key: " << data->getKeyName() << " Updating Value instead.";
            view_UpdateGraphMLData(item, data->getKeyName(), data->getValue());
        }else{
            //Add action!
            if(addAction){
                ActionItem action;
                action.ID = item->getID();
                action.actionType = CONSTRUCTED;
                action.actionKind = GraphML::DATA;
                action.keyName = data->getKeyName();
                addActionToStack(action);
            }
            item->attachData(data);
        }
    }
    return true;
}

bool NewController::attachGraphMLData(GraphML *item, GraphMLData *data, bool addAction)
{
    QList<GraphMLData *> dataList;
    dataList.append(data);
    return attachGraphMLData(item, dataList, addAction);
}

void NewController::addActionToStack(ActionItem action)
{
    //Get Current Action ID and action.
    action.actionID = currentActionID;
    action.actionName = currentAction;

    if(UNDOING){
        redoActionStack.push(action);
    }else{
        undoActionStack.push(action);
    }
    updateViewUndoRedoLists();
}

void NewController::undoRedo(bool undo)
{
    emit view_SetGraphMLSelected(0);

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
    emit view_SetGUIEnabled(false);

    //Get the ID and Name of the top-most action.
    int topActionID = actionStack.top().actionID;
    QString topActionName = actionStack.top().actionName;

    //Emit a new action so this Undo/Redo operation can be reverted.
    view_TriggerAction(topActionName);

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
    qCritical() << "Actions to Reverse: " << actionCount;
    for(int i = 0; i < actionCount; i++){
        int percentage = (i*100) / actionCount;
        emit view_UpdateProgressBar(percentage);
        reverseAction(toReverse[i]);
    }

    emit view_UpdateProgressBar(100);

    if(UNDOING){
        undoActionStack = actionStack;
    }else{
        redoActionStack = actionStack;
    }

    updateViewUndoRedoLists();
    emit view_SetGUIEnabled(true);

    UNDOING = false;
    REDOING = false;
}


void NewController::deleteSelectedNodes()
{
    emit view_SetGraphMLSelected(0);

    while(selectedNodeIDs.size() > 0){

        QString ID = selectedNodeIDs.front();
        Node* node = getNodeFromID(ID);

        Node* centeredNode = getNodeFromGraphML(centeredGraphML);


        if(node && node->isAncestorOf(centeredNode)){
            centeredNode = 0;
            qWarning() << "Unsetting Centered Node";
        }
        if(node){
            bool addAction = true;
            if(node->isInstance() || node->isImpl()){
                //addAction = false;
            }
            bool deleted = destructNode(node, addAction);

            if(!deleted){
                selectedNodeIDs.removeFirst();
            }
        }

    }
}

void NewController::deleteSelectedEdges()
{
    emit view_SetGraphMLSelected(0);

    while(selectedEdgeIDs.size() > 0){

        QString ID = selectedEdgeIDs.front();
        Edge* edge = getEdgeFromID(ID);

        if(edge){
            bool deleted = destructEdge(edge);

            if(!deleted){
                qCritical() << "Not Removed?!";
                selectedEdgeIDs.removeFirst();
            }
        }
    }

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

    if(node->getParentNode()){
        //Set the ParentNode ID if we have a Parent.
        action.parentID = node->getParentNode()->getID();
    }

    //Add Action to the Undo/Redo Stack.
    addActionToStack(action);

    //Add the node to the list of nodes constructed.
    storeGraphMLInHash(node);

    if(isNodeKindImplemented(node->getDataValue("kind"))){
        emit view_ConstructGraphMLGUI(node);
    }
}

void NewController::setupModel()
{
    constructNode(constructGraphMLDataVector("Model"));
    constructNodeGUI(model);

    GraphMLData* labelData = model->getData("label");
    connect(labelData, SIGNAL(dataChanged(GraphMLData*)), this, SLOT(view_ProjectNameUpdated(GraphMLData*)));

    //Construct the top level parents.
    constructChildNode(model, constructGraphMLDataVector("BehaviourDefinitions"));
    constructChildNode(model, constructGraphMLDataVector("InterfaceDefinitions"));
    constructChildNode(model, constructGraphMLDataVector("DeploymentDefinitions"));

    //Construct the second level containers.
    constructChildNode(deploymentDefinitions, constructGraphMLDataVector("AssemblyDefinitions"));
    constructChildNode(deploymentDefinitions, constructGraphMLDataVector("HardwareDefinitions"));

    //Update the Labels.
    behaviourDefinitions->updateDataValue("label", "Behaviour Definitions");
    interfaceDefinitions->updateDataValue("label", "Interface Definitions");
    deploymentDefinitions->updateDataValue("label", "Deployment Definitions");
    assemblyDefinitions->updateDataValue("label", "Assembly Definitions");
    hardwareDefinitions->updateDataValue("label", "Hardware Definitions");

    setupManagementComponents();

    //Sort the Model.
    //emit view_SortNode(assemblyDefinitions);
    //emit view_SortNode(deploymentDefinitions);
    //emit view_SortNode(model);
    //emit view_CenterGraphML(model);
    //emit view_FitToScreen();

    //Clear the Undo/Redo Stacks
    undoActionStack.clear();
    redoActionStack.clear();
}

void NewController::setupValidator()
{
    validator = new ValidationEngine();
    ValidationPlugin* interfacePlugin = new InterfaceDefinitionPlugin();
    connect(interfacePlugin, SIGNAL(highlightNodeError(Node*,QString)), this, SLOT(validator_HighlightError(Node*,QString)));
    validator->addPlugin(interfacePlugin);
}

void NewController::bindGraphMLData(Node *definition, Node *child)
{
    GraphMLData* typeData = 0;
    GraphMLData* labelData = 0;

    if(definition->getData("type")){
        typeData = definition->getData("type");
    }
    if(definition->getData("label")){
        labelData = definition->getData("label");
    }

    bool bindTypeToLabel = false;
    bool bindTypeToType = false;
    bool bindLabel = false;
    bool lockLabel = false;

    //If we have a Label and no Type for Definition, bind the Label of Inst to Definition Label.
    if(labelData && !typeData){
        bindTypeToLabel = true;
    }

    if(labelData && typeData){
        bindTypeToType = true;
    }

    if(child->getParentNode()->isInstance() || child->getParentNode()->isImpl() || child->isImpl()){
        bindLabel = true;
        lockLabel = true;
    }

   QStringList dataTypes;
   dataTypes << "type" << "label";

    //Bind the GraphMLData attached to the Definition to the Instance/Impl.
    foreach(GraphMLData* data, definition->getData()){
        GraphMLKey* dataKey = data->getKey();
        QString dataKeyName = data->getKeyName();
        QString dataValue = data->getValue();


        if(dataTypes.contains(dataKeyName)){ //!data->isProtected() || data->getParentData()){
            //Check for existing GraphMLData.
            GraphMLData* boundData = 0;
            boundData = child->getData(dataKey);

            if(!boundData){
                qCritical() << " No match for";
                //If the Instance/Impl does not have a matching GraphMLData, We have to attach one.
                boundData = new GraphMLData(dataKey, dataValue);
                //Attach the GraphMLData to the Instance/Impl
                attachGraphMLData(child, boundData, false);
            }

            if(dataKeyName == "type"){
                boundData->setProtected(true);
            }

            if(dataKeyName == "label"){
                if(!bindTypeToLabel || bindLabel){
                    data->bindData(boundData);

                    if(lockLabel){
                        boundData->setProtected(true);
                    }
                }
            }else{
                data->bindData(boundData);
            }
        }
    }


    if(bindTypeToLabel){
        GraphMLKey* typeKey = 0;
        typeKey = constructGraphMLKey("type", "string", "node");
        GraphMLData* childTypeData = 0;
        if(typeKey){
            //Set the type to be the value of the label from the definition
            childTypeData = child->getData(typeKey);
            if(!childTypeData){
                childTypeData = new GraphMLData(typeKey, labelData->getValue());

                if(childTypeData){
                    attachGraphMLData(child, childTypeData, false);
                }
            }
            if(childTypeData && labelData){
                childTypeData->setProtected(true);
                labelData->bindData(childTypeData);
            }
        }
    }



}

void NewController::setupDefinitionRelationship(Node *definition, Node *node, bool instance)
{
    if(!node || !definition){
        qCritical() << "setupNodeAsInstance() << Definition or Node is NULL.";
        return;
    }

    //Bind the un-protected GraphMLData attached to the Definition to the Instance.
    bindGraphMLData(definition, node);

    bool allCreated = true;

    //For each child contained in the Definition, which itself is a definition, construct an Instance/Impl inside the Parent Instance/Impl.
    foreach(Node* definitionChild, definition->getChildren(0)){
        if(definitionChild->isDefinition()){
            Node* childInstance = 0;
            childInstance = constructDefinitionRelative(node, definitionChild, instance);
            if(!childInstance){
                qCritical() << "Could not create a NodeInstance for: " << definitionChild->toString() << " In: " << node->toString();
                allCreated = false;
            }
        }
    }

    //If we have made all children, we can set this node as an instance of the definition.
    if(allCreated){
        if(instance){
            definition->addInstance(node);
        }else{
            definition->addImplementation(node);
        }
        //qDebug() << "Definition: " << definition->toString() << " added  Instance: " << node->toString();
    }else{
        qCritical() << "DEFINITION ERROR";
    }
}




void NewController::attachAggregateToEventPort(EventPort *eventPort, Aggregate *aggregate)
{
    //Must copy across a flattened aggreagte into eventPort;
    eventPort->setAggregate(aggregate);

    foreach(Node* implementation, eventPort->getImplementations()){
        if(implementation){
            constructDefinitionRelative(implementation, aggregate);
            qCritical() << "Implementation: " << implementation->toString() << " set Aggregate: " << aggregate->toString();
        }else{
            //qCritical() << eventPort->toString() << " Has no implementation!?";
        }
    }

}

void NewController::teardpwmDefinitionRelationship(Node *definition, Node *node, bool instance)
{
    if(!node || !definition){
        return;
    }

    //Unbind data.
    foreach(GraphMLData* attachedData, definition->getData()){
        GraphMLData* newData = 0;
        newData = node->getData(attachedData->getKey());
        if(newData){
            attachedData->unbindData(newData);
        }
    }

    if(definition->isConnected(node)){
        if(instance){
            definition->removeInstance(node);
        }else{
            definition->removeImplementation(node);
        }
        selectedNodeIDs.append(node->getID());
    }
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


    //Add Action to the Undo/Redo Stack
    if(!edge->wasGenerated()){
        addActionToStack(action);
    }

    //Get Source and Destination of the Edge.
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

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
            if(!edge->getData(descriptionKey)){
                GraphMLData* label = new GraphMLData(descriptionKey, "Instance Of");
                attachGraphMLData(edge, label, false);
            }
        }else if(dstKind + "Impl" == srcKind){
            //Got Implementation Edge
            setupDefinitionRelationship(dst, src, SETUP_AS_IMPL);
            if(!edge->getData(descriptionKey)){
                GraphMLData* label = new GraphMLData(descriptionKey, "Implementation Of");
                attachGraphMLData(edge, label, false);
            }
        }else if(edge->isAggregateLink()){
            EventPort* eventPort = dynamic_cast<EventPort*>(src);
            Aggregate* aggregate = dynamic_cast<Aggregate*>(dst);
            //Got Aggregate Edge.
            if(eventPort && aggregate){
                if(!edge->getData(descriptionKey)){
                    GraphMLData* label = new GraphMLData(descriptionKey, "Uses Aggregate");
                    attachGraphMLData(edge, label, false);
                }
                attachAggregateToEventPort(eventPort, aggregate);
            }
        }
    }

    if(edge->isDeploymentLink()){
        InEventPortInstance* iEPI = dynamic_cast<InEventPortInstance*>(dst);
        OutEventPortInstance* oEPI = dynamic_cast<OutEventPortInstance*>(src);
        if(iEPI && oEPI){
            //Bind Topics Together.
            GraphMLData* destinationTopicName = iEPI->getData("topicName");
            GraphMLData* sourceTopicName = oEPI->getData("topicName");
            if(destinationTopicName && sourceTopicName ){
                destinationTopicName->bindData(sourceTopicName);
                sourceTopicName->setProtected(true);
            }
        }
    }


    storeGraphMLInHash(edge);

    //Construct the GUI Representation of the Edge.
    emit view_ConstructGraphMLGUI(edge);
}

void NewController::updateViewUndoRedoLists()
{
    QList<int> actions;
    QStringList undoList;

    //Undo first
    foreach(ActionItem a, undoActionStack){
        int ID = a.actionID;
        if(actions.contains(ID) == false){
            actions.append(ID);
            undoList.insert(0, a.actionName);
        }
    }
    emit view_UpdateUndoList(undoList);

    undoList.clear();
    actions.clear();

    //Redo
    foreach(ActionItem a, redoActionStack){
        int ID = a.actionID;
        if(actions.contains(ID) == false){
            actions.append(ID);
            undoList.insert(0, a.actionName);
        }
    }

    emit view_UpdateRedoList(undoList);

}

void NewController::setupManagementComponents()
{

    GraphMLKey* typeKey = constructGraphMLKey("type", "string", "node");

    //EXECUTION MANAGER
    QList<GraphMLData*> executionManagerData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> dancePlanLauncherData = constructGraphMLDataVector("ManagementComponent") ;
    QList<GraphMLData*> ddsLoggingServerData = constructGraphMLDataVector("ManagementComponent") ;

    foreach(GraphMLData* data, executionManagerData){
        if(data->getKeyName() == "type"){
            data->setValue("DANCE_EXECUTION_MANAGER");
            data->setProtected(true);
        }
    }
    foreach(GraphMLData* data, dancePlanLauncherData){
        if(data->getKeyName() == "type"){
            data->setValue("DANCE_PLAN_LAUNCHER");
            data->setProtected(true);
        }
    }

    foreach(GraphMLData* data, ddsLoggingServerData){
        if(data->getKeyName() == "type"){
            data->setValue("DDS_LOGGING_SERVER");
            data->setProtected(true);
        }
    }


    Node* emNode = constructChildNode(assemblyDefinitions, executionManagerData);
    Node* plNode = constructChildNode(assemblyDefinitions, dancePlanLauncherData);
    Node* lsdNode = constructChildNode(assemblyDefinitions, ddsLoggingServerData);

    emNode->getData("label")->setValue("DANCE_EXECUTION_MANAGER");
    plNode->getData("label")->setValue("DANCE_PLAN_LAUNCHER");
    lsdNode->getData("label")->setValue("DDS_LOGGING_SERVER");
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

Model *NewController::getModel()
{
    return model;
}
