#include "newcontroller.h"

#include <QDebug>



NewController::NewController(NodeView *v)
{
    UNDOING = false;
    REDOING = false;
    SELECT_NEWLY_CREATED = false;
    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;


    HIDDEN_OPACITY = 0.10;


    //Attach the view.
    view = v;

    view->setController(this);

    model = 0;
    treeModel = new QStandardItemModel();

    //Construct

    behaviourDefinitions = 0;
    interfaceDefinitions = 0;
    deploymentDefinitions = 0;
    hardwareDefinitions = 0;
    assemblyDefinitions = 0;

    currentActionID = 0;
    actionCount = 0;
    currentAction = "";

    childNodeKind = "InEventPort";

    CUT_LINKING = false;
    centeredNode = 0;

    viewAspects << "Assembly" << "Workload" << "Definitions";
    protectedKeyNames << "x" << "y" << "kind"; //<< "width" << "height";


    containerNodeKinds << "Model";
    containerNodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    containerNodeKinds << "HardwareDefinitions" << "AssemblyDefinitions";

    constructableNodeKinds << "MemberInstance" << "AggregateInstance" << "AggregateMemberInstance";
    constructableNodeKinds << "File" << "Component" << "ComponentInstance" << "ComponentImpl";
    constructableNodeKinds << "Attribute" << "AttributeInstance" << "AttributeImpl";
    constructableNodeKinds << "InEventPort" << "InEventPortInstance" << "InEventPortImpl";
    constructableNodeKinds << "OutEventPort" << "OutEventPortInstance" << "OutEventPortImpl";
    constructableNodeKinds << "ComponentAssembly";
    constructableNodeKinds << "HardwareNode" << "HardwareCluster" ;
    constructableNodeKinds << "Member" << "Aggregate" << "AggregateMember";
    constructableNodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload";

    //Connect to the View's Signals
    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftPressed(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeletePressed(bool)));
    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(unselect()),this, SLOT(view_ClearSelection()));
    connect(view, SIGNAL(unselect()), this, SLOT(view_ClearCenteredNode()));
    connect(view, SIGNAL(constructNodeItem(QPointF)), this,SLOT(view_ConstructChildNode(QPointF)));


    //connect(view, SIGNAL(sortModel()),this, SLOT(view_SortModel()));
    connect(view, SIGNAL(escapePressed(bool)), this, SLOT(view_ClearSelection()));
    //Connect the controllers signals to the view.
    connect(this, SIGNAL(view_SetCentered(GraphML*)), view, SLOT(view_SetCentered(GraphML*)));
    connect(this, SIGNAL(view_SetRubberbandSelectionMode(bool)), view, SLOT(setRubberBandMode(bool)));


    connect(this, SIGNAL(view_PrintErrorCode(GraphML*,QString)), view, SLOT(printErrorText(GraphML*,QString)));
    //connect(this, SIGNAL(view_ConstructNodeGUI(Node*)), view, SLOT(view_ConstructNodeGUI(Node*)));

    setupModel();
    setupValidator();
    qCritical() << "Constructor Run!";
}

NewController::~NewController()
{
    view_ClearSelection();

    destructNode(model);
}

QString NewController::exportGraphML(QVector<Node *> eNodes)
{
    QString keyXML, edgeXML, nodeXML;
    QVector<Node*> containedNodes;
    QVector<GraphMLKey*> containedKeys;
    QVector<Edge*> containedEdges;

    //Get all Children and Edges.
    foreach(Node* node, eNodes){
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

    foreach(Node* node, eNodes){
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
    QVector<Node*> nodes;
    nodes.append(node);
    return exportGraphML(nodes);
}

QStandardItemModel *NewController::getModel()
{
    return treeModel;
}

QStringList NewController::getNodeKinds()
{
    return constructableNodeKinds;
}

QStringList NewController::getViewAspects()
{
    return viewAspects;
}

void NewController::view_ImportGraphML(QStringList inputGraphML, Node *currentParent)
{
    view_ActionTriggered("Importing GraphML");
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
    //qCritical() << "NewController::view_ImportGraphML()";

    //Key Lookup provides a way for the original key "id" to be linked with the internal object GraphMLKey
    QMap<QString , GraphMLKey*> keyLookup;

    //Node lookup provides a way for the original edge source/target ID's to be linked with the internal object Node
    QMap<QString, Node *> nodeLookup;

    //A list for storing the current Parse <data> tags owned by a <node>
    QVector<GraphMLData*> currentNodeData;

    //A list storing all the Edge information (source, target, data)
    QVector<EdgeTemp> currentEdges;
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
        currentParent = getParentModel();
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
        emit view_UpdateProgressBar((int)percentage);

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

                Node* newNode = constructNodeChild(currentParent, currentNodeData);
                //Node* newNode = constructGraphMLNode(currentNodeData, currentParent);

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
                    linkOldIDtoNewID(nodeID, newNode->getID());
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
    QVector<EdgeTemp> aggregateEdges;
    QVector<EdgeTemp> instanceEdges;
    QVector<EdgeTemp> implEdges;
    QVector<EdgeTemp> otherEdges;

    for(int i =0; i < currentEdges.size(); i++){
        EdgeTemp edge = currentEdges[i];
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];

        if(s && d){
            //qCritical() << "Edge: " << s->toString() << " to " << d->toString();
            if(s->isDefinition() || d->isDefinition()){
                if(s->isInstance() || d->isInstance()){
                    //qWarning() << "\tInstance Edge";
                    instanceEdges.append(edge);
                    continue;
                }else if(s->isImpl() || d->isImpl()){
                    //qWarning() << "\t Impl Edge";
                    implEdges.append(edge);
                    continue;
                }else if(s->getDataValue("kind") == "Aggregate" || d->getDataValue("kind") == "Aggregate" ){
                    //qWarning() << "\tAggregate Edge";
                    aggregateEdges.append(edge);
                    continue;
                }
            }
            //qWarning() << "\tNormal Edge";
            otherEdges.append(edge);
            //qWarning() << "";
        }
    }

    QVector<EdgeTemp> allEdges;
    allEdges << implEdges;
    allEdges << instanceEdges;
    allEdges << aggregateEdges;
    allEdges << otherEdges;

    //Construct the Edges from the EdgeTemp objects
    for(int i =0; i< allEdges.size(); i++){
        EdgeTemp edge = allEdges[i];
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];

        if(s && d){
            if(d->isDefinition()){
                view_ConstructEdge(d, s, edge.data, edge.id);
            }else{
                view_ConstructEdge(s, d, edge.data, edge.id);

            }
        }else{
            qCritical() << "Edge is Illegal!";
            //return false;
        }
    }
    emit view_UpdateProgressBar(100);
}

void NewController::validator_HighlightError(Node *node, QString error)
{
    if(node){
        view->view_SetCentered(node);
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

           qCritical() << "Removed Data: " << keyName << " from ID: " << action.ID;
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

void NewController::view_ConstructComponentInstance(Component *definition)
{
    if(!definition){
        definition = dynamic_cast<Component*>(getSelectedNode());
    }

    if(definition){
        view_ActionTriggered("Constructing Component Instance.");
        Node* node = constructNodeInstance(assemblyDefinitions, definition);
        if(node){
            view->view_SetCentered(node);
        }
    }
}

void NewController::view_ConstructComponentImpl(Component *definition)
{
    if(!definition){
        definition = dynamic_cast<Component*>(getSelectedNode());
    }

    if(definition){
        view_ActionTriggered("Constructing Component Implementation.");
        Node* node = constructNodeImplementation(behaviourDefinitions, definition);
        view->view_SetCentered(node);
    }
}


void NewController::view_CenterComponentImpl(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && node->getImplementation()){
        view->view_SetCentered(node->getImplementation());
    }
}

void NewController::view_CenterComponentDefinition(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && node->getDefinition()){
        view->view_SetCentered(node->getDefinition());
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
            view->view_SetCentered(eP->getAggregate());
        }
    }
}

void NewController::view_ProjectNameUpdated(GraphMLData *label)
{
    emit view_UpdateProjectName(label->getValue());
}


void NewController::view_ConstructMenu(QPoint position)
{
    QPoint globalPos = view->mapToGlobal(position);

    menuPosition = view->mapToScene(position);

    Node* node = getSelectedNode();




    QMenu* rightClickMenu = new QMenu();

    QAction* deleteAction = new QAction(this);
    deleteAction->setText("Delete Selection");
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(view_DeletePressed()));

    QAction* addChildNode = new QAction(this);
    addChildNode->setText("Add Child Node");
    connect(addChildNode, SIGNAL(triggered()), this, SLOT(view_ConstructChildNode()));


    if(node){
        QString kind = node->getDataValue("kind");
        if(node->isDefinition() && (!node->getParentNode()->isDefinition())){
            QAction* createInstance = new QAction(this);
            createInstance->setText("Create Component Instance");
            connect(createInstance, SIGNAL(triggered()), this, SLOT(view_ConstructComponentInstance()));
            rightClickMenu->addAction(createInstance);
        }

        if(node->isDefinition() && !node->getImplementation() && (!node->getParentNode()->isDefinition())){
            QAction* createInstance = new QAction(this);
            createInstance->setText("Create Component Implementation");
            connect(createInstance, SIGNAL(triggered()), this, SLOT(view_ConstructComponentImpl()));
            rightClickMenu->addAction(createInstance);
        }

        rightClickMenu->addSeparator();

        if((node->isInstance() || node->isDefinition()) && node->getImplementation()){
            QAction* gotoAction = new QAction(this);
            gotoAction->setText("Center " + kind + " Implementation");
            connect(gotoAction, SIGNAL(triggered()), this, SLOT(view_CenterComponentImpl()));
            rightClickMenu->addAction(gotoAction);
        }

        if((node->isInstance() || node->isImpl()) && node->getDefinition()){
            QAction* gotoAction = new QAction(this);
            gotoAction->setText("Center " + kind + " Definition");
            connect(gotoAction, SIGNAL(triggered()), this, SLOT(view_CenterComponentDefinition()));
            rightClickMenu->addAction(gotoAction);
        }

        if(((node->isInstance() || node->isImpl()) && node->getDefinition()) || node->isDefinition()){
            QAction* gotoAction = new QAction(this);
            gotoAction->setText("Center " + kind + " Aggregate");
            connect(gotoAction, SIGNAL(triggered()), this, SLOT(view_CenterAggregate()));
            rightClickMenu->addAction(gotoAction);
        }


        rightClickMenu->addSeparator();
    }



    rightClickMenu->addAction(deleteAction);
    rightClickMenu->addAction(addChildNode);

    rightClickMenu->exec(globalPos);
}


void NewController::view_ExportGraphML(QString filename)
{

    QVector<Node*> nodes;
    nodes.append(model);

    /*
    foreach(Node* child, getParentModel()->getChildren(0)){
        Node* node = dynamic_cast<Node*>(child);
        if(node){
            nodes.append(node);
        }
    }
    */

    QString data = exportGraphML(nodes);

    emit view_WriteGraphML(filename, data);
}

void NewController::view_SetNodeSelected(Node *node, bool setSelected)
{
    //Check for Null Nodes.
    if(!node){
        return;
    }

    if(KEY_CONTROL_DOWN){
        //If it contains node, unselect it.
        //Else append it to the list of selected.
        bool setSelected = !isNodeSelected(node);
        setNodeSelected(node, setSelected);
    }else{
        //If Shift is down, Construct an Edge between the selected Node and the previously selected Node.
        if(KEY_SHIFT_DOWN){
            Node* src = getSelectedNode();
            if(selectedEdges.size() == 0 && src){
                view_ActionTriggered("Constructing Edge!");
                view_ConstructEdge(src, node);
            }
        }
        //If node isn't selected, and Shift and Control aren't pressed, clear selection and set this node as selected.
        if(!isNodeSelected(node)){
            //Clear selected items
            view_ClearSelection();
            setNodeSelected(node, setSelected);
        }
    }

}

void NewController::view_SetEdgeSelected(Edge *edge, bool setSelected)
{
    qCritical() << "view_SetEdgeSelected";

    if(KEY_CONTROL_DOWN){
        //If it contains node, unselect it.
        bool setSelected = !isEdgeSelected(edge);
        setEdgeSelected(edge, setSelected);
    }else{
        if(!isEdgeSelected(edge)){
            //Clear selected items
            view_ClearSelection();
            setEdgeSelected(edge, setSelected);
        }
    }

}

void NewController::view_SetItemSelected(GraphML *item, bool setSelected)
{
    if(!item){
        return;
    }

    if(isGraphMLNode(item)){
        view_SetNodeSelected(getNodeFromGraphML(item), setSelected);
    }
    if(isGraphMLEdge(item)){
        view_SetEdgeSelected(getEdgeFromGraphML(item), setSelected);
    }

}


void NewController::view_ConstructChildNode(QPointF centerPoint)
{
    qCritical() << "Making Child Node";

    bool okay;
    QInputDialog* dialog = new QInputDialog();

    //Get the current Selected Node.
    Node* parent = getSelectedNode();
    if(!parent){
        parent = getParentModel();
    }

    if(parent->isInstance()){
        qCritical() << "Cannot Import or Paste into an Instance!";
        return;
    }

    QString nodeKind = dialog->getItem(0, "Selected Node Type","Please Select Child Node Type: ",this->getAdoptableNodeKinds(parent),0,false, &okay);

    if(!okay){
        return;
    }

    delete dialog;

    view_ActionTriggered("Constructing Child Node");
    constructNodeChild(parent, constructGraphMLDataVector(nodeKind,centerPoint));
}

void NewController::view_ConstructChildNode()
{
    //Invoked through Menu
    view_ConstructChildNode(menuPosition);
}

void NewController::view_ConstructEdge(Node *src, Node *dst)
{
    QVector<QStringList> noData;

    view_ConstructEdge(src, dst, noData);
}

void NewController::view_ConstructEdge(Node *src, Node *dst, QVector<GraphMLData *> data, QString previousID)
{

    Edge* edge = constructEdge(src, dst);
    if(edge){
        attachGraphMLData(edge, data);

        //If we are Undo-ing or Redo-ing and we have an ID to link it to.
        if((UNDOING || REDOING) && previousID != ""){
            linkOldIDtoNewID(previousID, edge->getID());
        }
        storeGraphMLInHash(edge);
        constructEdgeGUI(edge);
    }
}

void NewController::view_ConstructEdge(Node *src, Node *dst, QVector<QStringList> attachedData, QString previousID)
{
    Edge* edge = constructEdge(src, dst);
    if(edge){
         attachGraphMLData(edge, attachedData);

         if((UNDOING || REDOING) && previousID != ""){
             linkOldIDtoNewID(previousID, edge->getID());
         }
         storeGraphMLInHash(edge);

         constructEdgeGUI(edge);

    }

    if(!src->isConnected(dst)){
            qCritical() << "Edge: " << src->toString() << " to " << dst->toString() << " not legal.";
            qCritical() << "Edge not legal";
    }
}

void NewController::view_MoveSelectedNodes(QPointF delta)
{
    foreach(Node* node, selectedNodes){
        GraphMLData* xData = node->getData("x");
        GraphMLData* yData = node->getData("y");

        float x = xData->getValue().toFloat() + delta.x();
        float y = yData->getValue().toFloat() + delta.y();

        view_UpdateGraphMLData(node,"x", QString::number(x));
        view_UpdateGraphMLData(node,"y", QString::number(y));
    }

}

void NewController::view_SetChildNodeKind(QString nodeKind)
{

    childNodeKind = nodeKind;
}

void NewController::view_FilterNodes(QStringList filterString)
{
//    foreach(Node* node, nodes){
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
                view->view_SetOpacity(node, 1);
            }else{
                view->view_SetOpacity(node, 0);
            }


    }
}

void NewController::view_HideUnconnectableNodes(Node *src)
{
    foreach(QString ID, nodeIDs){
        Node* dst = getNodeFromID(ID);
        if(!isEdgeLegal(src, dst) && (dst != src)){
            view->view_SetOpacity(dst, HIDDEN_OPACITY);
        }
    }
}

void NewController::view_ShowAllNodes()
{
    foreach(Node* node, nodes){
        view->view_SetOpacity(node, 1);
    }
}

void NewController::view_ActionTriggered(QString actionName)
{
    actionCount++;
    currentAction = actionName;
    currentActionID = actionCount;
}

void NewController::view_ClearHistory()
{
    undoStack.clear();
    redoStack.clear();
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
            view_HideUnconnectableNodes(node);
        }
    }else{
        view_ShowAllNodes();

    }

}

void NewController::view_DeletePressed(bool isDown)
{
    qCritical() << "Delete";
     if(isDown){
         emit view_SetGUIEnabled(false);
         view_ActionTriggered("Deleting Selection");
         //Do Delete
         deleteSelectedEdges();
         deleteSelectedNodes();
         emit view_SetGUIEnabled(true);
     }else{
         qCritical() << "GG;";
     }
}

void NewController::view_Undo()
{
    UNDOING = true;
    undoRedo();
    UNDOING = false;
}

void NewController::view_Redo()
{
    REDOING = true;
    undoRedo();
    REDOING = false;
}

void NewController::view_Copy()
{
    copySelectedNodesGraphML();
}

void NewController::view_Cut()
{
    //Run Copy
    if(copySelectedNodesGraphML()){
        emit view_SetGUIEnabled(false);
        view_ActionTriggered("Cutting Selection.");
        deleteSelectedEdges();
        deleteSelectedNodes();
        CUT_LINKING = true;
        emit view_SetGUIEnabled(true);
    }
}

void NewController::view_Paste(QString xmlData)
{

    if(isGraphMLValid(xmlData) && xmlData != ""){
        //Paste it into the current Selected Node,
        Node* node = getSelectedNode();
        if(node){
            qCritical() << node->toString();
        }

        //Paste into the current Maximized Node.
        if(centeredNode != 0){
            node = centeredNode;
        }

        //Clear Selection.
        clearSelectedEdges();
        clearSelectedNodes();


        SELECT_NEWLY_CREATED = true;
        view_ActionTriggered("Pasting Selection.");
        view_ImportGraphML(xmlData, node, CUT_LINKING);
        CUT_LINKING = false;
        SELECT_NEWLY_CREATED = false;
    }
}

void NewController::view_ClearKeyModifiers()
{
}

void NewController::view_SelectAll()
{
    qCritical() << "NewController::view_SelectAll";

    clearSelectedEdges();
    clearSelectedNodes();

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
        foreach(Node* child, getParentModel()->getChildren(0)){
            Node* node = dynamic_cast<Node*>(child);
            if(node){
                if(node == behaviourDefinitions || node == deploymentDefinitions || node == interfaceDefinitions){
                    foreach(Node* childN, node->getChildren(0)){
                        Node* nodeN = dynamic_cast<Node*>(childN);
                        setNodeSelected(nodeN);
                    }
                }else{
                    setNodeSelected(node);
                }
            }

        }
    }

}

void NewController::view_ClearCenteredNode()
{
    centeredNode = 0;
}

void NewController::view_ClearSelection()
{
    clearSelectedEdges();
    clearSelectedNodes();
}

bool NewController::copySelectedNodesGraphML()
{
    Node* firstParent = 0;

    foreach(Node* node, selectedNodes){
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
    QString result = exportGraphML(selectedNodes);

    //Tell the view to place the resulting GraphML String into the Copy buffer.
    emit view_UpdateCopyBuffer(result);
    return true;
}

QStringList NewController::getAdoptableNodeKinds(Node *parent)
{
    QStringList adoptableNodeTypes;

    foreach(QString nodeKind, getNodeKinds()){
        //Construct a Node of the Kind nodeKind.
        Node* node = constructNode(constructGraphMLDataVector(nodeKind));

        //If we have constructed this node, test if the parent can adopt it.
        if(node && parent->canAdoptChild(node)){
            adoptableNodeTypes.append(nodeKind);
        }

        //Delete the node, if we didn't create a GUI NodeItem.
        if(node && !view->guiCreated(node)){
            delete node;
        }
    }

    return adoptableNodeTypes;
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


Node *NewController::constructNode(QVector<GraphMLData *> dataToAttach)
{
    //The to-be-constructed node.
    Node* node;

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

    QString nodeKind = "";

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

    if(nodeKind == ""){
        qCritical() << "Cannot Construct a Kind-less Node.";
        return 0;
    }

    //Attach blank data to the Vector for the Unset Data Keys.
    if(nodeKind != "Model"){
        if(setWidth){
            dataToAttach.append(new GraphMLData(widthKey, ""));
        }
        if(setHeight){
            dataToAttach.append(new GraphMLData(heightKey, ""));
        }
        if(setX){
            dataToAttach.append(new GraphMLData(xKey, ""));
        }
        if(setY){
            dataToAttach.append(new GraphMLData(yKey, ""));
        }
        if(setLabel){
            dataToAttach.append(new GraphMLData(labelKey, "new_" + nodeKind));
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
    }else if(nodeKind == "ComponentAssembly"){
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
    }else if(nodeKind == "InEventPort"){
        node = new InEventPort();
    }else if(nodeKind == "InEventPortInstance"){
        node = new InEventPortInstance();
    }else if(nodeKind == "InEventPortImpl"){
        node = new InEventPortImpl();
    }else if(nodeKind == "Attribute"){
        node = new Attribute();
    }else if(nodeKind == "AttributeInstance"){
        node = new AttributeInstance();
    }else if(nodeKind == "AttributeImpl"){
        node = new AttributeImpl();
    }else if(nodeKind == "HardwareNode"){
        node = new HardwareNode();
    }else if(nodeKind == "HardwareCluster"){
        node = new HardwareCluster();
    }else if(nodeKind == "File"){
        node = new File();
    }else if(nodeKind == "Member"){
        node = new Member();
    }else if(nodeKind == "Aggregate"){
        node = new Aggregate();
    }else if(nodeKind == "AggregateMember"){
        node = new AggregateMember();
    }else if(nodeKind == "AggregateMemberInstance"){
        node = new AggregateMemberInstance();
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

    //Adds the data to the newly created Node.
    //This will add Undo states to reverse.
    attachGraphMLData(node, dataToAttach);

    node->addAspect("Definitions");
    return node;
}

Edge *NewController::constructEdge(Node *source, Node *destination)
{
    if(!source || !destination){
        qCritical() << "Source or Destination Node is Null!";
        return 0;
    }

    if(isEdgeLegal(source, destination)){
        QString sourceKind = source->getDataValue("kind");
        QString destinationKind = source->getDataValue("kind");

        Edge* edge = 0;
        //Make sure that the edge is the correct way around.
        if((sourceKind == "Aggregate" && destinationKind != "AggregateInstance") || destination->isDefinition()){
            edge = new Edge(destination, source);
        }else{
            edge = new Edge(source, destination);
        }

        return edge;
    }else{
        qCritical() << "Edge: Source: " << source->toString() << " to Destination: " << destination->toString() << " Cannot be created!";
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
        qCritical() << "Cannot find GraphML from Lookup Hash. ID: " << ID;
    }
    return 0;
}

void NewController::removeGraphMLFromHash(QString ID)
{
    if(IDLookupGraphMLHash.contains(ID)){
        qCritical() << "Hash Removed ID: " << ID;
        GraphML* item = IDLookupGraphMLHash[ID];

        IDLookupGraphMLHash.remove(ID);


        if(item->getKind() == GraphML::NODE){
            nodeIDs.removeOne(ID);
        }else if(item->getKind() == GraphML::EDGE){
            edgeIDs.removeOne(ID);
        }
        qCritical() << IDLookupGraphMLHash.size();
        qCritical() << (nodeIDs.size() + edgeIDs.size());
    }
}

Node *NewController::constructNodeChild(Node *parentNode, QVector<GraphMLData *> dataToAttach)
{
    //Construct the Model Node first.
    Node* node = constructNode(dataToAttach);

    //Check if this node has been already setup Visually.
    if(view->guiCreated(node)){
        //If it has, we shouldn't rebuild it.
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

        storeGraphMLInHash(node);
    }else{
        qCritical() << "Node: " << parentNode->toString() << " Cannot Adopt: " << node->toString();
        //Delete the newly created node.
        delete node;
        node = 0;
    }

    //If the node is a defintion, construct an instance/Implementation in each Instance/Implementation of the parentNode.
    if(node && node->isDefinition()){
        foreach(Node* child, parentNode->getInstances()){
            constructNodeInstance(child, node);
        }
        foreach(Node* child, parentNode->getImplementations()){
            constructNodeImplementation(child, node);
        }
    }

    return node;
}

QVector<GraphMLData *> NewController::constructGraphMLDataVector(QString nodeKind, QPointF relativePosition)
{
    GraphMLKey* kindKey = constructGraphMLKey("kind", "string", "node");
    GraphMLKey* xKey = constructGraphMLKey("x", "double", "node");
    GraphMLKey* yKey = constructGraphMLKey("y", "double", "node");

    QVector<GraphMLData*> data;

    data.append(new GraphMLData(kindKey, nodeKind));
    data.append(new GraphMLData(xKey, QString::number(relativePosition.x())));
    data.append(new GraphMLData(yKey, QString::number(relativePosition.y())));

    return data;
}

QVector<GraphMLData *> NewController::constructGraphMLDataVector(QString nodeKind)
{
    //No Position provided, so call with a Blank Point.
    return constructGraphMLDataVector(nodeKind, QPointF(0,0));
}

Node *NewController::constructNodeInstance(Node *parent, Node *definition)
{
    //Construct a List of GraphMLData derived from the Definition to bind to the new Instance.
    QVector<GraphMLData *> matchableData;


    QStringList requiredKeyNames;
    requiredKeyNames << "kind" << "label" << "x" << "y";

    //Clone the Data for an Instance.
    foreach(GraphMLData* data, definition->getData()){
        GraphMLKey* key = data->getKey();
        QString keyName = key->getName();
        QString keyValue = data->getValue();

        if(requiredKeyNames.contains(keyName)){
            QString modifier = "";

            if(keyName == "kind"){
                //AggregateInstance and MemberInstances are able to Instance themselves.
                if(keyValue != "AggregateInstance" && keyValue != "MemberInstance"){
                    modifier = "Instance";
                }
            }
            GraphMLData* newData = new GraphMLData(key, keyValue + modifier);
            newData->setProtected(true);
            matchableData.append(newData);
        }
    }

    Node* newNode = 0;

    QStringList matchKeyNames;
    matchKeyNames << "kind" << "label";

    //Look for children which match.
    foreach(Node* child, parent->getChildren(0)){
        if(child->getDefinition() && child->getDefinition() == definition){
            //Already got a Definition.
            //qCritical() << "Definition is already connected.";
            newNode = child;
            return newNode;
        }

        bool allDataMatched = true;

        foreach(GraphMLData* data, matchableData){
             GraphMLKey* key = data->getKey();
             QString keyName = key->getName();

             //Get the GraphMLData for the child, matching the key.
             GraphMLData* childData = child->getData(key);

             if(!childData){
                 //No Match.
                 allDataMatched = false;
                 break;
             }

             if(matchKeyNames.contains(keyName)){
                 if(data->getValue() != childData->getValue()){
                     allDataMatched = false;
                     break;
                 }
             }
        }

        if(allDataMatched){
            //Found a match.
            newNode = child;
            break;
        }
    }

    if(!newNode){
        qCritical() << "No Matches found, So Creating Instance.";
        qCritical() << "In: " <<  parent->toString() << "" << definition->toString();
        newNode = constructNodeChild(parent, matchableData);
    }

    if(newNode){
        //Construct an Edge
        view_ConstructEdge(definition, newNode);
    }
    return newNode;
}

Node *NewController::constructNodeImplementation(Node *parent, Node *definition)
{
    QVector<GraphMLData *> data;

    foreach(GraphMLData* attachedData, definition->getData()){
        if(attachedData->getKeyName() == "kind" || attachedData->getKeyName() == "label"){
            QString modifier = "";
            if(attachedData->getKeyName() == "kind"){
                modifier += "Impl";
            }
            GraphMLData* kindData = new GraphMLData(attachedData->getKey(), attachedData->getValue() + modifier);

            kindData->setProtected(true);

            data.append(kindData);
        }
    }

    //Check for Duplicates.
    Node* impl = 0;

    foreach(Node* child, parent->getChildren(0)){

        bool allMatched = true;
        foreach(GraphMLData* attachedData, data){
            GraphMLKey* key = attachedData->getKey();
            QString keyName = attachedData->getKeyName();
            QString value = attachedData->getValue();

            GraphMLData* matchingData = child->getData(key);
            if(child->getDefinition()){
                allMatched = false;
                break;
            }

            if(!matchingData){
                allMatched = false;
                break;
            }else{
                if(keyName == "kind" && value != matchingData->getValue() ){
                    allMatched = false;
                    break;
                }
                if(keyName == "label" && value != matchingData->getValue()){
                    allMatched = false;
                    break;
                }
            }
        }

        if(allMatched){
            impl = child;
        }
    }


    if(!impl){
        qCritical() << "Making impl";
        impl = constructNodeChild(parent, data);
        //impl =  constructGraphMLNode(data, parent);
    }

    view_ConstructEdge(definition, impl);
    return impl;
}

void NewController::clearSelectedNodes()
{
    foreach(Node* node, selectedNodes){
        view->view_SetSelected(node, false);
    }
    selectedNodes.clear();

    view->scene()->update();
}

void NewController::clearSelectedEdges()
{
    foreach(Edge* edge, selectedEdges){
        view->view_SetSelected(edge, false);
    }
    selectedEdges.clear();
    view->scene()->update();
}

void NewController::setNodeSelected(Node *node, bool setSelected)
{
    if(!view->guiCreated(node)){
        qCritical() << "View hasn't got a GUI Component for Node.";
        return;
    }

    if(setSelected){
        //Check to see if Node's Parents are in the list of selected Nodes.
        if(!isNodesAncestorSelected(node)){
            //Unselect any children nodes which are contained by the new node.
            foreach(Node* selectedNode, selectedNodes){
                if(node->isAncestorOf(selectedNode)){
                    setNodeSelected(selectedNode, false);
                }
            }

            view->view_SetSelected(node, true);

            selectedNodes.append(node);
        }

        //Check all selected Edges.
        foreach(Edge* selectedEdge, selectedEdges){
            Node* src = selectedEdge->getSource();
            Node* dst = selectedEdge->getDestination();

            if(node == src || node == dst){
                setEdgeSelected(selectedEdge, false);
            }
        }
    }else{
        view->view_SetSelected(node, false);

        //Remove from list.
        int position = selectedNodes.indexOf(node);
        if(position >= 0){
            selectedNodes.removeAt(position);
        }
    }

    view->scene()->update();
}

void NewController::setEdgeSelected(Edge *edge, bool setSelected)
{
    //TODO: Implement Control Multiple Selection.

    if(!view->guiCreated(edge)){
        qCritical() << "View hasn't got a GUI Component for Edge.";
        return;
    }

    if(setSelected){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        //If Either ends of the Edge's Nodes are not selected.
        if(!isNodeSelected(src) && !isNodeSelected(dst)){
            if(edge->isInstanceLink()){
                //Select all Children Instance Links between src and dst.
                QVector<Edge*> dstEdges = dst->getEdges(-1);

                foreach(Edge* childEdge, dstEdges){
                    if(childEdge->isInstanceLink()){
                        if(src->isAncestorOf(childEdge->getSource())){
                            view->view_SetSelected(childEdge, true);
                            selectedEdges.append(childEdge);
                        }
                    }
                }
            }

            view->view_SetSelected(edge, true);
            selectedEdges.append(edge);
        }
    }else{
        view->view_SetSelected(edge, false);
        //Remove from list.
        int position = selectedEdges.indexOf(edge);
        if(position != -1){
            selectedEdges.removeAt(position);
        }
    }
    view->scene()->update();
}

Node *NewController::getSelectedNode()
{
    if(selectedNodes.size() == 1){
        return selectedNodes[0];
    }

    return 0;
}

Edge *NewController::getSelectedEdge()
{
    if(selectedEdges.size() == 1){
        return selectedEdges[0];
    }
    return 0;
}

bool NewController::destructNode(Node *node)
{
    if(node){
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

        //Delete any first level children which are instances.

        QVector<Node*> children = node->getChildren();
        QVector<Edge*> childEdges = node->getEdges();

        QString nodesXML = exportGraphML(node);


        if(node->getParentNode()){
            if((node->getParentNode()->isInstance() || node->getParentNode()->isImpl()) && node->getParentNode()->getDefinition() != 0 && node->getDefinition()){
               if(node->getDefinition()){
                   //Cannot delete Child node.
                   qCritical() << "Cannot delete Instance's inheritted child. Must be deleted from Definition.";
                   return false;
               }
            }
        }


        //Remove all Edges First.
        foreach(Edge* edge, childEdges){
            Node* src = (Node*) edge->getSource();
            Node* dst = (Node*) edge->getDestination();
            //If this node contains both Edge ends, then it will be contained by the XML exported.
            if(node->isAncestorOf(src) && node->isAncestorOf(dst)){
                destructEdge(edge, false);
            }else{
                //If it doesn't contain both ends, we need to add an action to reconnect the Edge.
                destructEdge(edge, true);
            }
        }


        //Remove all Children Nodes
        foreach(Node* child, children){
            Node* childNode = dynamic_cast<Node*>(child);
            if(childNode){
                //Remove node from list.
                int nodePosition = nodes.indexOf(childNode);
                int selectedPosition = selectedNodes.indexOf(childNode);

                if(nodePosition != -1){
                    nodes.removeAt(nodePosition);
                }
                if(selectedPosition != -1){
                    selectedNodes.removeAt(selectedPosition);
                }

                view->view_DestructGraphMLGUI(childNode);
            }
        }


        //Add an action to reverse this action.
        ActionItem action;
        action.actionKind = GraphML::NODE;
        action.actionType = DESTRUCTED;
        action.removedXML = nodesXML;
        action.ID =  node->getID();
        if(node->getParentNode()){
            action.parentID = node->getParentNode()->getID();
        }

        addActionToStack(action);

        //Remove node from list.
        int nodePosition = nodes.indexOf(node);
        int selectedPosition = selectedNodes.indexOf(node);

        if(nodePosition != -1){
            nodes.removeAt(nodePosition);
        }
        if(selectedPosition != -1){
            selectedNodes.removeAt(selectedPosition);
        }

        view->view_DestructGraphMLGUI(node);

        removeGraphMLFromHash(action.ID);
        delete node;
        return true;
    }else{
        qCritical() << "Node doesn't exist!!";
        return false;
    }
}

bool NewController::destructEdge(Edge *edge, bool addAction)
{
    if(edge){
        if(addAction){
            ActionItem action;
            action.actionType = DESTRUCTED;
            action.actionKind = GraphML::EDGE;
            action.ID = edge->getID();

            action.srcID = edge->getSource()->getID();
            action.dstID = edge->getDestination()->getID();

            foreach(GraphMLData* data, edge->getData()){
                action.dataValues.append(data->toStringList());
            }

            addActionToStack(action);
        }


        if(edge->isInstanceLink()){
            //qCritical() << "Got Instance Link";
            //Delete Instances/Impls
            teardownNodeAsInstance(edge->getSource(), edge->getDestination());
        }
        if(edge->isImplLink()){
            //qCritical() << "Got Impl Link";
            //Delete Instances/Impls
            teardownNodeAsImplementation(edge->getSource(), edge->getDestination());
        }

        if(edge->isAggregateLink()){
            //qCritical() << "Got Aggregate Link";

            Aggregate* aggregate = dynamic_cast<Aggregate*>(edge->getDestination());
            EventPort* eP = dynamic_cast<EventPort*>(edge->getSource());
            if(eP){
                //qCritical() << "Unsetting Aggregate";
                eP->unsetAggregate();
            }

            foreach(Node* impl, eP->getImplementations()){
                foreach(Node* child, impl->getChildren(0)){
                    if(child->isConnected(aggregate)){
                        //qCritical() << "Got Aggregate Child!";
                        child->setDefinition(0);
                        setNodeSelected(child, true);
                    }
                }
            }
        }

        //Remove Edge from list.
        int position = edges.indexOf(edge);
        if(position >= 0){
            edges.removeAt(position);
        }

        //Remove Edge from selectedEdges list
        position = selectedEdges.indexOf(edge);
        if(position >= 0){
            selectedEdges.removeAt(position);
        }

        view->view_DestructGraphMLGUI(edge);

        removeGraphMLFromHash(edge->getID());
        delete edge;
        return true;
    }else{
         qCritical() << "Edge doesn't exist!!";
         return false;

    }
}

bool NewController::isNodesAncestorSelected(Node *selectedNode)
{
    for(int i = 0; i < selectedNodes.size(); i++){
        Node* node = selectedNodes[i];
        if(node && node->isAncestorOf(selectedNode)){
            return true;
        }
    }
    return false;
}

bool NewController::isNodeSelected(Node *node)
{
    return selectedNodes.contains(node);
}

bool NewController::isEdgeSelected(Edge *edge)
{
    return selectedEdges.contains(edge);
}

bool NewController::isEdgeLegal(Node *src, Node *dst)
{
    //Check for Valid Nodes.
    if(src && dst){
        //Check for dual way connections.
        return src->canConnect(dst) && dst->canConnect(src);
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
            destructNode(node);
            break;
        }
        case GraphML::EDGE:{
            //Delete Edge.
            Edge* edge = getEdgeFromID(action.ID);
            destructEdge(edge, true);
            break;
        }
        case GraphML::DATA:{
            //Delete Data
            GraphML* item = getGraphMLFromID(action.ID);
            view_DestructGraphMLData(item, action.keyName);
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
            view_ImportGraphML(action.removedXML, parentNode, true);
            break;
        }
        case GraphML::EDGE:{
            //Get Source and Destination, attempt to construct an Edge.
            Node* src = getNodeFromID(action.srcID);
            Node* dst = getNodeFromID(action.dstID);

            if(isEdgeLegal(src,dst)){
                view_ConstructEdge(src ,dst, action.dataValues, action.ID);
            }
            break;
        }
        case GraphML::DATA:{
            GraphML* attachedItem = getGraphMLFromID(action.ID);

            if(attachedItem){
                bool success = attachGraphMLData(attachedItem, action.dataValues);
            }else{
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
bool NewController::attachGraphMLData(GraphML *item, QVector<QStringList> dataList)
{
    //Attach the data.
    QVector<GraphMLData*> graphMLData;
    foreach(QStringList data, dataList){
        if(data.size() == 4){
            QString keyName = data.at(0);
            QString keyType = data.at(1);
            QString keyFor = data.at(2);
            QString dataValue = data.at(3);

            GraphMLKey* key = constructGraphMLKey(keyName, keyType, keyFor);
            if(key){
                GraphMLData* data = new GraphMLData(key, dataValue);
                if(data){
                    graphMLData.append(data);
                    continue;
                }else{
                    qCritical() << "GraphML data cannot be added!";
                }
            }
        }
        qCritical() << "Data Illegal";
    }
    return attachGraphMLData(item, graphMLData);
}

bool NewController::attachGraphMLData(GraphML *item, QVector<GraphMLData *> dataList)
{
    if(!item){
        qCritical() << "Null GraphML Item.";
        return false;
    }

    foreach(GraphMLData* data, dataList){
        GraphMLData* containedData = item->getData(data->getKey());
        if(containedData){
            //Update so we have an Undo.
            //qWarning() << item->toString() << " Found duplicate Data for key: " << data->getKeyName() << " Updating Value instead.";
            view_UpdateGraphMLData(item, data->getKeyName(), data->getValue());
        }else{
            //We are adding new data to an existing
            if(view->guiCreated(item)){
                //Add action!
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

bool NewController::attachGraphMLData(GraphML *item, GraphMLData *data)
{
    QVector<GraphMLData *> dataList;
    dataList.append(data);
    return attachGraphMLData(item, dataList);
}

void NewController::addActionToStack(ActionItem action)
{
    //Get Current Action ID and action.
    action.actionID = currentActionID;
    action.actionName = currentAction;

    if(UNDOING){
        redoStack.push(action);
    }else{
        undoStack.push(action);
    }
    updateViewUndoRedoLists();
}

void NewController::undoRedo()
{
    //Lock the GUI.
    emit view_SetGUIEnabled(false);

    //Used to store the stack of actions we are to use.
    QStack<ActionItem> actionStack = redoStack;

    if(UNDOING){
        //Set to the use the stack.
        actionStack = undoStack;
    }

    //Get the total number of actions in the history stack.
    float actionCount = actionStack.size();

    if(actionCount == 0){
        qCritical () << "No Actions in Undo/Redo Buffer.";
        return;
    }

    //Get the ID and Name of the top-most action.
    int topActionID = actionStack.top().actionID;
    QString topActionName = actionStack.top().actionName;

    //Emit a new action so this Undo/Redo operation can be reverted.
    view_ActionTriggered(topActionName);

    //This vector will store all ActionItems which match topActionID
    QVector<ActionItem> toReverse;

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
    for(int i = 0; i < actionCount; i++){
        int percentage = (i*100) / actionCount;
        emit view_UpdateProgressBar(percentage);
        reverseAction(toReverse[i]);
    }

    emit view_UpdateProgressBar(100);

    if(UNDOING){
        undoStack = actionStack;
    }else{
        redoStack = actionStack;
    }

    updateViewUndoRedoLists();
    emit view_SetGUIEnabled(true);
}


void NewController::deleteSelectedNodes()
{
    while(selectedNodes.size() > 0){
        Node* node = selectedNodes.front();
        if(node && node->isAncestorOf(centeredNode)){
            centeredNode = 0;
            qWarning() << "Unsetting Centered Node";
        }
        bool deleted = destructNode(node);


        if(!deleted){
            if(view->guiCreated(node)){
                view->view_SetSelected(node, false);
                selectedNodes.removeFirst();
            }
        }
    }
}

void NewController::deleteSelectedEdges()
{
    foreach(Edge* edge, selectedEdges){
        destructEdge(edge);
    }
    selectedEdges.clear();
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
    nodes.append(node);

    if(isNodeKindImplemented(node->getDataValue("kind"))){
        view->view_ConstructNodeGUI(node);
    }
}

void NewController::setupModel()
{
    //Construct the GraphMLData for the Model.
    QVector<GraphMLData *> data = constructGraphMLDataVector("Model");

    //Construct a Label Data for the Model.
    GraphMLKey* labelKey = constructGraphMLKey("label", "string", "node");
    GraphMLData* labelData = new GraphMLData(labelKey, "New Project");
    data.append(labelData);

    //Connect the Label dataChanging Signal with the ProjectName changed slot.
    connect(labelData, SIGNAL(dataChanged(GraphMLData*)), this, SLOT(view_ProjectNameUpdated(GraphMLData*)));

    //Construct a Node for the Model.
    Node* modelNode = constructNode(data);

    //Store in the Lookup
    storeGraphMLInHash(modelNode);

    //Construct the GUI for the Model.
    constructNodeGUI(modelNode);

    //Construct the top level parents.
    constructNodeChild(model, constructGraphMLDataVector("BehaviourDefinitions"));
    constructNodeChild(model, constructGraphMLDataVector("InterfaceDefinitions"));
    constructNodeChild(model, constructGraphMLDataVector("DeploymentDefinitions"));

    //Construct the second level containers.
    constructNodeChild(deploymentDefinitions, constructGraphMLDataVector("AssemblyDefinitions"));
    constructNodeChild(deploymentDefinitions, constructGraphMLDataVector("HardwareDefinitions"));

    //Update the Labels.
    behaviourDefinitions->updateDataValue("label", "Behaviour Definitions");
    interfaceDefinitions->updateDataValue("label", "Interface Definitions");
    deploymentDefinitions->updateDataValue("label", "Deployment Definitions");
    assemblyDefinitions->updateDataValue("label", "Assembly Definitions");
    hardwareDefinitions->updateDataValue("label", "Hardware Definitions");

    //Sort the Model.
    view->view_SortNode(model);

    //Clear the Undo/Redo Stacks
    undoStack.clear();
    redoStack.clear();
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
    //Bind the un-protected GraphMLData attached to the Definition to the Instance/Impl.
    foreach(GraphMLData* data, definition->getData()){
        GraphMLKey* dataKey = data->getKey();
        QString dataValue = data->getValue();

        if(!data->isProtected()){
            //Check for existing GraphMLData.
            GraphMLData* boundData = child->getData(dataKey);

            if(!boundData){
                //If the Instance/Impl does not have a matching GraphMLData, We have to attach one.
                boundData = new GraphMLData(dataKey, dataValue);
                //Attach the GraphMLData to the Instance/Impl
                attachGraphMLData(child, boundData);

                if(child->isInstance() && definition->getDataValue("kind") == "Component" && dataKey->getName() == "label"){
                    //If the Definition is a Component, and the GraphMLData is for the label
                    //and the child is an Instance, Then don't protect it. Deployments can have their own unique names.
                    boundData->setProtected(false);
                }else{
                    //Protect the Data.
                    boundData->setProtected(true);
                }
            }
            //Bind the Instance/Impl GraphMLData to the Defintion GraphMLData.
            data->bindData(boundData);
        }
    }
}

void NewController::setupNodeAsInstance(Node *definition, Node *node)
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
            Node* childInstance = constructNodeInstance(node, definitionChild);
            if(!childInstance){
                qCritical() << "Could not create a NodeInstance for: " << definitionChild->toString() << " In: " << node->toString();
                allCreated = false;
            }
        }
    }

    //If we have made all children, we can set this node as an instance of the definition.
    if(allCreated){
        definition->addInstance(node);
        qDebug() << "Definition: " << definition->toString() << " added  Instance: " << node->toString();
    }
}

void NewController::setupNodeAsImplementation(Node *definition, Node *node)
{
    if(!node || !definition){
        qCritical() << "setupNodeAsImplementation() << Definition or Node is NULL.";
        return;
    }

    //Bind the un-protected GraphMLData attached to the Definition to the Impl.
    bindGraphMLData(definition, node);

    bool allCreated = true;

    //For each child contained in the Definition, which itself is a definition, construct an Instance/Impl inside the Parent Instance/Impl.
    foreach(Node* definitionChild, definition->getChildren(0)){
        if(definitionChild->isDefinition()){
            Node* childImpl = constructNodeImplementation(node, definitionChild);
            if(!childImpl){
                qCritical() << "Could not create a NodeImpl for: " << definitionChild->toString() << " In: " << node->toString();
                allCreated = false;
            }
        }
    }

    //If we have made all children, we can set this node as an implementation of the definition.
    if(allCreated){
        definition->addImplementation(node);
        qDebug() << "Definition: " << definition->toString() << " added  Implementation: " << node->toString();
    }
}


void NewController::attachAggregateToEventPort(EventPort *eventPort, Aggregate *aggregate)
{
    //Must copy across a flattened aggreagte into eventPort;
    eventPort->setAggregate(aggregate);

    foreach(Node* implementation, eventPort->getImplementations()){
        if(implementation){
            constructNodeInstance(implementation, aggregate);
            qCritical() << "Implementation: " << implementation->toString() << " set Aggregate: " << aggregate->toString();
        }else{
            qCritical() << eventPort->toString() << " Has no implementation!?";
        }
    }

}

void NewController::teardownNodeAsImplementation(Node *definition, Node *implementation)
{
    //qCritical() << "Tearing Down Impl: " << definition->toString() << " <-> " << implementation->toString();
    if(!implementation || !definition){
        return;
    }

    //Unbind data.
    foreach(GraphMLData* attachedData, definition->getData()){
        GraphMLData* newData = implementation->getData(attachedData->getKey());
        if(newData){
            attachedData->unbindData(newData);
        }
    }

    if(definition->isConnected(implementation)){
        definition->removeImplementation(implementation);
        if(!selectedNodes.contains(implementation)){
            selectedNodes.push_front(implementation);
        }
    }

}

void NewController::teardownNodeAsInstance(Node *definition, Node *instance)
{
    if(!instance || !definition){
        return;
    }

    //Unbind data.
    foreach(GraphMLData* attachedData, definition->getData()){
        GraphMLData* newData = instance->getData(attachedData->getKey());
        if(newData){
            attachedData->unbindData(newData);
        }
    }

    if(definition->isConnected(instance)){
        definition->removeInstance(instance);
        if(!selectedNodes.contains(instance)){
            selectedNodes.push_front(instance);
        }
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
            qCritical() << "isGraphMLValid(): Parsing Error! Line #" << lineNumber;
            qCritical() << "\t" << xmlErrorChecking.errorString();
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

    //Add Action to the Undo/Redo Stack
    addActionToStack(action);

    //Get Source and Destination of the Edge.
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    //qWarning() << "Setting Up Edge: " << "Source: " << src->toString() << " to Destination: " << dst->toString();

    //Check for the special Edges
    if(src->isDefinition()){
        //Get the Node Kind of the Source and Destination
        QString srcKind = src->getDataValue("kind");
        QString dstKind = dst->getDataValue("kind");
        if(srcKind == "" || dstKind == ""){
            qCritical() << "Got an undefined Node Kind.";
        }

        if(srcKind + "Instance" == dstKind || (srcKind == dstKind)){
            //Got Instance Edge.
            //If the source and destination are the same type, it must be an Aggregate Instance or MemberInstance.
            setupNodeAsInstance(src, dst);
        }else if(srcKind + "Impl" == dstKind){
            //Got Implementation Edge
            setupNodeAsImplementation(src, dst);
        }else if(edge->isAggregateLink()){
            EventPort* eventPort = dynamic_cast<EventPort*>(src);
            Aggregate* aggregate = dynamic_cast<Aggregate*>(dst);
            //Got Aggregate Edge.
            if(eventPort && aggregate){
                attachAggregateToEventPort(eventPort, aggregate);
            }
        }
    }

    //Add the edge to the list of edges constructed.
    edges.append(edge);

    //Construct the GUI Representation of the Edge.
    view->view_ConstructEdgeGUI(edge);
}

void NewController::updateViewUndoRedoLists()
{
    QVector<int> actions;
    QStringList undoList;

    //Undo first
    foreach(ActionItem a, undoStack){
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
    foreach(ActionItem a, redoStack){
        int ID = a.actionID;
        if(actions.contains(ID) == false){
            actions.append(ID);
            undoList.insert(0, a.actionName);
        }
    }

    emit view_UpdateRedoList(undoList);

}

GraphML *NewController::getGraphMLFromID(QString ID)
{
    //Check for old IDs
    ID = getNewIDFromPreviousID(ID);

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

Node *NewController::getNodeFromPreviousID(QString ID)
{
    ID = getNewIDFromPreviousID(ID);

    Node* node = getNodeFromID(ID);

    return node;
}

QString NewController::getNewIDFromPreviousID(QString ID)
{
    QString originalID = ID;
    QString newID = ID;
    while(newID != ""){
         if(IDLookupHash.contains(ID)){
             QString temp = ID;
             ID = newID;
             newID = IDLookupHash[temp];
         }else{
             break;
         }
    }

    if(ID != originalID){
        //qDebug() << "Looking for ID: " <<originalID << " Found ID:" << ID;
    }
    return ID;

}

GraphML *NewController::getGraphMLFromPreviousID(QString ID)
{
    ID = getNewIDFromPreviousID(ID);
    return getGraphMLFromHash(ID);
}

void NewController::linkOldIDtoNewID(QString previousID, QString newID)
{
    IDLookupHash[previousID] = newID;
    if(!IDLookupHash.contains(newID)){
        IDLookupHash[newID] = "";
    }

    qDebug() << "Linked old ID:" << previousID << " to new ID:" << newID;

}

bool NewController::isGraphMLNode(GraphML *item)
{
    return getNodeFromGraphML(item) != 0;
}

bool NewController::isGraphMLEdge(GraphML *item)
{
    return getEdgeFromGraphML(item) != 0;
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

Model *NewController::getParentModel()
{
    return model;
}


