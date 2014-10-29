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

    model = 0;
    treeModel = new QStandardItemModel();

    //Construct

    behaviourDefinitions = 0;
    interfaceDefinitions = 0;
    deploymentDefinitions = 0;

    currentActionID = 0;
    actionCount = 0;
    currentAction = "";

    childNodeKind = "InEventPort";

    CUT_LINKING = false;
    centeredNode = 0;

    viewAspects << "Assembly" << "Workload" << "Definitions";
    protectedKeyNames << "x" << "y" << "kind"; //<< "width" << "height";

    nodeKinds << "Model";
    nodeKinds << "BehaviourDefinitions" << "DeploymentDefinitions" << "InterfaceDefinitions";
    nodeKinds << "File" << "Component" << "ComponentInstance" << "ComponentImpl";
    nodeKinds << "Attribute" << "AttributeInstance" << "AttributeImpl";
    nodeKinds << "InEventPort" << "InEventPortInstance" << "InEventPortImpl";
    nodeKinds << "OutEventPort" << "OutEventPortInstance" << "OutEventPortImpl";
    nodeKinds << "ComponentAssembly";
    nodeKinds << "HardwareNode" << "HardwareCluster" ;
    nodeKinds << "PeriodicEvent" << "Member" << "Aggregate" << "AggregateMember";


    //Connect to the View's Signals
    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftPressed(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeletePressed(bool)));
    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(unselect()),this, SLOT(view_ClearSelection()));
    connect(view, SIGNAL(unselect()), this, SLOT(view_ClearCenteredNode()));
    connect(view, SIGNAL(constructNodeItem(QPointF)), this,SLOT(view_ConstructChildNode(QPointF)));

    connect(view, SIGNAL(escapePressed(bool)), this, SLOT(view_ClearSelection()));
    //Connect the controllers signals to the view.
    connect(this, SIGNAL(view_SetNodeItemCentered(NodeItem*)), view, SLOT(centreItem(NodeItem*)));
    connect(this, SIGNAL(view_SetRubberbandSelectionMode(bool)), view, SLOT(setRubberBandMode(bool)));


    setupModel();
}

NewController::~NewController()
{
    emit view_SetSelectedAttributeModel(0);
    view_ClearSelection();
    view_SelectAll();
    view_DeletePressed(true);

    delete model;
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
    return nodeKinds;
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
    qCritical() << "NewController::view_ImportGraphML()";

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
                currentKey = parseGraphMLKeyXML(xml);

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
                Node* newNode = constructGraphMLNode(currentNodeData, currentParent);

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
                    linkPreviousIDToID(nodeID, newNode->getID());
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


    //Construct the Edges from the EdgeTemp objects
    for(int i =0; i< currentEdges.size(); i++){
        EdgeTemp edge = currentEdges[i];
        Node* s = nodeLookup[edge.source];
        Node* d = nodeLookup[edge.target];



        if(s != 0 && d != 0){
            view_ConstructEdge(s, d, edge.data, edge.id);
            /*
            if(s->isEdgeLegal(d) && d->isEdgeLegal(s)){
                //Construct the edge, and attach the data.
                Edge* newEdge = new Edge(s, d);
                newEdge->attachData(edge.data);
                setupEdge(newEdge);

                if(linkID){
                    linkPreviousIDToID(edge.id, newEdge->getID());
                }

            }else{
                qCritical() << QString("Line #%1: Edge Not Valid!").arg(QString::number(edge.lineNumber));
                //return false;
            }
                */
        }else{
            qCritical() << "Edge is Illegal!";
            //return false;
        }
    }
      emit view_UpdateProgressBar(100);
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


        //Update
        GraphMLData* data = parent->getData(keyName);


        if(data){
            action.dataValues.append(data->toStringList());
            action.boundDataIDs.append(data->getBoundIDS());

           QString parentDataID = "";
           if(data->getParentData()){
               parentDataID = data->getParentData()->getID();
           }
           action.parentDataID.append(parentDataID);

           qCritical() << "Removed Data: " << keyName << " from ID: " << action.ID;
           addActionToStack(action);

            //Remove!
            parent->removeData(data);
            delete data;
        }else{
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
        Node* node = constructNodeInstance(deploymentDefinitions, definition, true);
        view_SetNodeCentered(node);
    }
}

void NewController::view_ConstructComponentImpl(Component *definition)
{
    if(!definition){
        definition = dynamic_cast<Component*>(getSelectedNode());
    }

    if(definition){
        view_ActionTriggered("Constructing Component Implementation.");
        Node* node = constructNodeImpl(behaviourDefinitions, definition, true);
        view_SetNodeCentered(node);
    }
}

void NewController::view_CenterComponentImpl(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && node->getImplementation()){
        view_SetNodeCentered(node->getImplementation());
    }
}

void NewController::view_CenterComponentDefinition(Node *node)
{
    if(!node){
        node = getSelectedNode();
    }
    if(node && node->getDefinition()){
        view_SetNodeCentered(node->getDefinition());
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
        if(eP){
            qCritical() << eP->toString();
            qCritical() << eP->getAggregate()->toString();
            view_SetNodeCentered(eP->getAggregate());
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

    QAction* selectedItem = rightClickMenu->exec(globalPos);
    /*
    if (selectedItem)
    {
        if(selectedItem->text() == "Delete Selection"){
            emit deletePressed(true);
        }else if(selectedItem->text() == "Add Child Node"){
            emit constructNodeItem(scenePos);
        }
        // something was chosen, do stuff
    }
    else
    {
        // nothing was chosen
    }
    */

}


void NewController::view_ExportGraphML(QString filename)
{
    QVector<Node*> nodes;

    foreach(Node* child, getParentModel()->getChildren(0)){
        Node* node = dynamic_cast<Node*>(child);
        if(node){
            nodes.append(node);
        }
    }

    QString data = exportGraphML(nodes);

    emit view_WriteGraphML(filename, data);
}

void NewController::view_SetNodeSelected(Node *node, bool setSelected)
{
    if(!node){
        return;
    }

    if(KEY_CONTROL_DOWN){
        //If it contains node, unselect it.
        bool setSelected = !isNodeSelected(node);
        setNodeSelected(node, setSelected);
    }else{
        if(KEY_SHIFT_DOWN){
            Node* src = getSelectedNode();
            if(selectedEdges.size() == 0 && src != 0){
                view_ActionTriggered("Constructing Edge!");
                view_ConstructEdge(src, node);
            }
        }
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

    if(selectedNodes.size() == 1 && selectedEdges.size() == 0){
        Node* node = getSelectedNode();
        GraphMLItem* gui = getNodeItemFromNode(node);
        emit view_SetSelectedAttributeModel(gui->getAttributeTable());
    }
    if(selectedEdges.size() == 1 && selectedNodes.size() == 0){
        Edge* edge = getSelectedEdge();
        GraphMLItem* gui = getNodeEdgeFromEdge(edge);
        emit view_SetSelectedAttributeModel(gui->getAttributeTable());
    }



}

void NewController::view_SetNodeCentered(Node *node)
{
    qCritical() << "view_SetNodeCentered";
    centeredNode = node;
    NodeItem* nodeItem = getNodeItemFromNode(node);
    if(nodeItem){
        clearSelectedNodes();
        emit view_SetNodeItemCentered(nodeItem);
    }
}

void NewController::view_ConstructChildNode(QPointF centerPoint)
{
    qCritical() << "Making Child Node";

    bool okay;
    QInputDialog *dialog = new QInputDialog();
    QString nodeType = dialog->getItem(0, "Selected Node Type","Please Select Child Node Type: ",this->getNodeKinds(),0,false, &okay);

    if(!okay){
        return;
    }


    //Get the current Selected Node.
    Node* parent = getSelectedNode();
    if(!parent){
        parent = getParentModel();
    }else{

        if(parent->isInstance() || parent->isImpl()){
            qCritical() << "Cannot Import or Copy and Paste into a Instace/Impl";
            return;
        }

        NodeItem* nodeItem = getNodeItemFromNode(getSelectedNode());
        if(nodeItem){
            centerPoint = nodeItem->mapFromScene(centerPoint);
        }
    }

    view_ActionTriggered("Constructing Child Node");
    constructNode(parent, nodeType, centerPoint);
}

void NewController::view_ConstructChildNode()
{
    //Invoked through Menu
    view_ConstructChildNode(menuPosition);
}

void NewController::view_ConstructEdge(Node *src, Node *dst)
{
    QVector<QStringList> noData;
    view_ConstructEdge(src, dst, noData, 0);
}

void NewController::view_ConstructEdge(Node *src, Node *dst, QVector<GraphMLData *> data, QString previousID)
{
    if(isEdgeLegal(src, dst)){
        Edge* edge = new Edge(src, dst);

        attachGraphMLData(edge, data);

        if((UNDOING || REDOING) && previousID != 0){
            linkPreviousIDToID(previousID, edge->getID());
        }

        setupEdge(edge);
    }else{
        qCritical() << "Edge not legal";
    }
}

void NewController::view_ConstructEdge(Node *src, Node *dst, QVector<QStringList> attachedData, QString previousID)
{
    if(isEdgeLegal(src, dst)){
        Edge* edge = new Edge(src, dst);

        attachGraphMLData(edge, attachedData);

        if((UNDOING || REDOING) && previousID != 0){
            linkPreviousIDToID(previousID, edge->getID());
        }

        setupEdge(edge);

    }else{
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
    foreach(Node* node, nodes){
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

        NodeItem* nodeItem = getNodeItemFromNode(node);
        if(allMatched){
            nodeItem->setOpacity(1);
        }else{
            nodeItem->setOpacity(0);
        }

    }
}

void NewController::view_HideUnconnectableNodes(Node *src)
{
    foreach(Node* dst, nodes){
        if(!isEdgeLegal(src, dst) && (dst != src)){
            NodeItem* nodeItem = getNodeItemFromNode(dst);
            if(nodeItem){
                nodeItem->setOpacity(HIDDEN_OPACITY);
            }
        }
    }
}

void NewController::view_ShowAllNodes()
{
    foreach(Node* node, nodes){
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if(nodeItem && nodeItem->opacity() != 1){
            nodeItem->setOpacity(1);
        }
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
    copySelection();
}

void NewController::view_Cut()
{
    //Run Copy
    if(copySelection()){
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
    //Clear Model

    emit view_SetSelectedAttributeModel(getNodeItemFromNode(model)->getAttributeTable());

}

void NewController::view_ConstructNodeItem(Node *node)
{
    if(node == 0){
        qCritical() << "Node is Null";
        return;
    }

    QString nodeKind = node->getDataValue("kind");

    //If we are meant to make this node.
    if(isNodeKindImplemented(nodeKind)){
        //Get Visual Parent Node

        Node* parentNode = node->getParentNode();
        NodeItem* parentNodeItem = getNodeItemFromNode(parentNode);
        if(parentNode == model){
            qCritical() << (parentNodeItem == 0);
        }

        NodeItem* nodeItem = new NodeItem(node, parentNodeItem);

        nodeItems.append(nodeItem);
        view->addNodeItem(nodeItem);

        //connect(nodeItem, SIGNAL(setNodeSelected(Node*, bool)),this, SLOT(view_SetNodeSelected(Node*,bool)));
        connect(nodeItem, SIGNAL(setItemSelected(GraphML*,bool)), this, SLOT(view_SetItemSelected(GraphML*,bool)));
        connect(nodeItem, SIGNAL(actionTriggered(QString)), this, SLOT(view_ActionTriggered(QString)));

        connect(nodeItem, SIGNAL(centreNode(Node*)), this, SLOT(view_SetNodeCentered(Node*)));
        connect(nodeItem, SIGNAL(makeChildNode(QPointF)), this,SLOT(view_ConstructChildNode(QPointF)));
        connect(nodeItem, SIGNAL(moveSelection(QPointF)), this, SLOT(view_MoveSelectedNodes(QPointF)));

        connect(node, SIGNAL(destroyed()), nodeItem, SLOT(destructNodeItem()));
        connect(this, SIGNAL(view_SetRubberbandSelectionMode(bool)),nodeItem, SLOT(setRubberbandMode(bool)));


        connect(nodeItem, SIGNAL(destructGraphMLData(GraphML*,QString)), this, SLOT(view_DestructGraphMLData(GraphML*,QString)));
        connect(nodeItem, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), this, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));


        //setNodeSelected(node, SELECT_NEWLY_CREATED);
    }else{
        qCritical() << "GraphMLController::model_MadeNodeNew() << Node Kind: " << nodeKind << " not Implemented";
    }

}

void NewController::view_ConstructNodeEdge(Edge *edge)
{
    Node* src = (Node*) edge->getSource();
    Node* dst = (Node*) edge->getDestination();

    NodeItem* srcGUI = getNodeItemFromNode(src);
    NodeItem* dstGUI = getNodeItemFromNode(dst);

    if(srcGUI != 0 && dstGUI != 0){
        //We have valid GUI elements for both ends of this edge.
        //Make an action for this Operation.

        //Construct a new GUI Element for this edge.
        NodeEdge* nodeEdge = new NodeEdge(edge, srcGUI, dstGUI);

        //Add it to the list of EdgeItems in the Model.
        nodeEdges.append(nodeEdge);

        connect(edge, SIGNAL(destroyed()), nodeEdge, SLOT(destructNodeEdge()));
        connect(nodeEdge, SIGNAL(actionTriggered(QString)), this, SLOT(view_ActionTriggered(QString)));
        connect(nodeEdge, SIGNAL(setItemSelected(GraphML*,bool)), this, SLOT(view_SetItemSelected(GraphML*,bool)));

        connect(nodeEdge, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), this, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));

       // connect(nodeEdge, SIGNAL(setSelected(Edge*,bool)), this, SLOT(view_SetEdgeSelected(Edge*,bool)));

        view->addEdgeItem(nodeEdge);
    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }
}

bool NewController::copySelection()
{
    //COPY ONLY WORKS IF YOU SHARE THE SAME PARENT.
    Node* parent = 0;
    foreach(Node* node, selectedNodes){
        if(!parent){
            parent = node->getParentNode();
        }else{
            if(parent != node->getParentNode()){
                qCritical() << "Can only copy items which share the same parent";
                return false;
            }
        }
    }

    //Export the GraphML for those Nodes.
    QString result = exportGraphML(selectedNodes);

    //Tell the view to place the resulting GraphML String into the Copy buffer.
    emit view_UpdateCopyBuffer(result);
    return true;
}

QString NewController::getXMLAttribute(QXmlStreamReader &xml, QString attrID)
{
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attrID)){
        return attributes.value(attrID).toString();
    }else{
        qCritical() << "Expecting Attribute key" <<attrID;
        return "";
    }

}

GraphMLKey *NewController::parseGraphMLKeyXML(QXmlStreamReader &xml)
{
    QString name = getXMLAttribute(xml,"attr.name");
    QString typeStr = getXMLAttribute(xml,"attr.type");
    QString forStr = getXMLAttribute(xml,"for");

    return constructGraphMLKey(name,typeStr,forStr);

}

Node *NewController::constructGraphMLNode(QVector<GraphMLData *> data, Node *parent)
{
    Node *newNode;


    GraphMLKey* w = constructGraphMLKey("width", "double", "node");
    GraphMLKey* h = constructGraphMLKey("height", "double", "node");
    GraphMLKey* x = constructGraphMLKey("x", "double", "node");
    GraphMLKey* y = constructGraphMLKey("y", "double", "node");

    bool noWidth = true;
    bool noHeight = true;
    bool noX = true;
    bool noY = true;
    QString kind;

    //Get kind from nodeData.
    for(int i=0; i < data.size(); i++){
        GraphMLData* currentData = data[i];
        QString keyName = currentData->getKeyName();
        QString keyData = currentData->getValue();


        if(keyName == "kind"){
            kind = keyData;
        }
        if(currentData->getKey() == w){
            noWidth = false;
        }
        if(currentData->getKey() == h){
            noHeight = false;
        }
        if(currentData->getKey() == x){
            noX = false;
        }
        if(currentData->getKey() == y){
            noY = false;
        }
    }

    if(kind != "Model"){
        if(noWidth){
            data.append(new GraphMLData(w, ""));
        }
        if(noHeight){
            data.append(new GraphMLData(h, ""));
        }
        if(noX){
            data.append(new GraphMLData(x, ""));
        }
        if(noY){
            data.append(new GraphMLData(y, ""));
        }
    }


    QVector<QString> aspects;


    if(kind == "Model"){
        newNode = new Model();
        aspects << "Definitions" << "Assembly" << "Workload";

    }else if(kind == "Component"){
        newNode = new Component();
        aspects << "Definitions";

    }else if(kind == "ComponentInstance"){
        newNode = new ComponentInstance();
        aspects << "Assembly";

    }else if(kind == "ComponentImpl"){
        newNode = new ComponentImpl();
        aspects << "Workload";

    }else if(kind == "OutEventPort"){
        newNode = new OutEventPort();
        aspects << "Definitions";

    }else if(kind == "OutEventPortInstance"){
        newNode = new OutEventPortInstance();
        aspects << "Assembly";

    }else if(kind == "OutEventPortImpl"){
        newNode = new OutEventPortImpl();
        aspects << "Workload";

    }else if(kind == "InEventPort"){
        newNode = new InEventPort();
        aspects << "Definitions";

    }else if(kind == "InEventPortInstance"){
        newNode = new InEventPortInstance();
        aspects << "Assembly";

    }else if(kind == "InEventPortImpl"){
        newNode = new InEventPortImpl();
        aspects << "Workload";

    }else if(kind == "Attribute"){
        newNode = new Attribute();
        aspects << "Definitions";

    }else if(kind == "AttributeInstance"){
        newNode = new AttributeInstance();
        aspects << "Assembly";

    }else if(kind == "AttributeImpl"){
        newNode = new AttributeImpl();
        aspects << "Workload";
    }
    else if(kind == "HardwareNode"){
        newNode = new HardwareNode();
        aspects << "Assembly";

    }else if(kind == "HardwareCluster"){
        newNode = new HardwareCluster();
        aspects << "Assembly";

    }else if(kind == "PeriodicEvent"){
        newNode = new PeriodicEvent();
        aspects << "Workload" ;

    }else if(kind == "File"){
        newNode = new File();
        aspects << "Definitions";

    }else if(kind == "Member"){
        newNode = new Member();
        aspects << "Definitions";

    }else if(kind == "Aggregate"){
        newNode = new Aggregate();
        aspects << "Definitions";

    }else if(kind == "AggregateMember"){
        newNode = new AggregateMember();
        aspects << "Definitions";

    }else if(kind == "BehaviourDefinitions"){
        if(behaviourDefinitions){
            return behaviourDefinitions;
        }else{
            behaviourDefinitions = new BehaviourDefinitions();
            newNode = behaviourDefinitions;
        }
        aspects << "Workload";

    }else if(kind == "DeploymentDefinitions"){
        if(deploymentDefinitions){
            return deploymentDefinitions;
        }else{
            deploymentDefinitions = new DeploymentDefinitions();
            newNode = deploymentDefinitions;
        }
        aspects << "Assembly";

    }else if(kind == "InterfaceDefinitions"){
        if(interfaceDefinitions){
            return interfaceDefinitions;
        }else{
            interfaceDefinitions = new InterfaceDefinitions();
            newNode = interfaceDefinitions;
        }
        aspects << "Definitions";


    }else{
        //qCritical() << "Kind:" << kind << "Not implemented";
        newNode = new BlankNode();
        aspects << "Workload" << "Assembly";
    }

    foreach(QString aspect, aspects){
        newNode->addAspect(aspect);
    }



    newNode->attachData(data);


    //Adopt the new Node into the parent
    if(!parent && model){
        parent = model;
        qCritical() << "Using Model as Parent";
    }



    if(!model && kind=="Model"){
        qCritical() << "Got Model";
        setupNode(newNode);
        return newNode;
    }

    if(parent->canAdoptChild(newNode)){
        parent->addChild(newNode);
        setupNode(newNode);
    }else{
        qCritical() << "Parent cannot Adopt Child";
        //Delete the newly created node.
        delete newNode;
        return 0;
    }


    if(newNode->isDefinition() && parent->isDefinition()){
        foreach(Node* child, parent->getInstances()){
            constructNodeInstance(child, newNode);
        }
        if(parent->getImplementation()){
            constructNodeImpl(parent->getImplementation(), newNode);
        }
    }


    return newNode;
}

GraphMLKey *NewController::constructGraphMLKey(QString name, QString type, QString forString)
{
    GraphMLKey *attribute = new GraphMLKey(name, type, forString);


    for(int i = 0 ; i < keys.size(); i ++){
        if(keys[i]->operator ==(*attribute)){
            delete attribute;
            return this->keys[i];
        }
    }

    if(protectedKeyNames.contains(name)){
        attribute->setDefaultProtected(true);
    }
    keys.append(attribute);

    return attribute;
}

Node *NewController::constructNode(Node *parent, QString kind, QPointF position)
{
    GraphMLKey* x = constructGraphMLKey("x", "double", "node");
    GraphMLKey* y = constructGraphMLKey("y", "double", "node");


    GraphMLKey* k = constructGraphMLKey("kind", "string", "node");
    GraphMLKey* t = constructGraphMLKey("type", "string", "node");
    GraphMLKey* l = constructGraphMLKey("label", "string", "node");

    QVector<GraphMLData *> data;

    data.append(new GraphMLData(x, QString::number(position.x())));
    data.append(new GraphMLData(y, QString::number(position.y())));
    data.append(new GraphMLData(k, kind));
    data.append(new GraphMLData(t, kind));
    data.append(new GraphMLData(l, "new_" + kind ));




    return constructGraphMLNode(data, parent);
}

Node *NewController::constructNodeInstance(Node *parent, Node *definition, bool forceCreate)
{

    QVector<GraphMLData *> data;


    foreach(GraphMLData* attachedData, definition->getData()){
        if(attachedData->getKeyName() == "kind" || attachedData->getKeyName() == "label"){
            QString modifier = "";
            if(attachedData->getKeyName() == "kind"){
                modifier += "Instance";
            }
            GraphMLData* kindData = new GraphMLData(attachedData->getKey(), attachedData->getValue() + modifier);

            kindData->setProtected(true);

            data.append(kindData);
        }
    }


    //Check for Duplicates.
    Node* instance = 0;

    if(!forceCreate){
        foreach(Node* child, parent->getChildren(0)){

            bool allMatched = true;
            foreach(GraphMLData* attachedData, data){
                GraphMLKey* key = attachedData->getKey();
                QString value = attachedData->getValue();

                bool gotMatch = false;

                foreach(GraphMLData* existingData, child->getData()){
                    if(key == existingData->getKey() && value == existingData->getValue()){
                        gotMatch = true;
                        break;
                    }
                }
                if(!gotMatch){
                    allMatched = false;
                }
            }
            if(allMatched){
                instance = dynamic_cast<Node*>(child);
            }
        }
    }

    if(!instance){
        instance =  constructGraphMLNode(data, parent);
    }

    view_ConstructEdge(definition, instance);
    return instance;
}

Node *NewController::constructNodeImpl(Node *parent, Node *definition, bool forceCreate)
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

    if(!forceCreate){
        foreach(Node* child, parent->getChildren(0)){

            bool allMatched = true;
            foreach(GraphMLData* attachedData, data){
                GraphMLKey* key = attachedData->getKey();
                QString value = attachedData->getValue();

                bool gotMatch = false;

                foreach(GraphMLData* existingData, child->getData()){
                    if(key == existingData->getKey() && value == existingData->getValue()){
                        gotMatch = true;
                        break;
                    }
                }
                if(!gotMatch){
                    allMatched = false;
                }
            }
            if(allMatched){
                impl = child;
            }
        }
    }

    if(!impl){
        impl =  constructGraphMLNode(data, parent);
    }

    view_ConstructEdge(definition, impl);
    return impl;
}

void NewController::clearSelectedNodes()
{
    foreach(Node* node, selectedNodes){
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if(nodeItem != 0){
            nodeItem->setSelected(false);
        }
    }
    selectedNodes.clear();

    view->scene()->update();
}

void NewController::clearSelectedEdges()
{
    foreach(Edge* edge, selectedEdges){
        NodeEdge* nodeEdge = getNodeEdgeFromEdge(edge);
        if(nodeEdge != 0){
            nodeEdge->setSelected(false);
        }
    }
    selectedEdges.clear();
    view->scene()->update();
}

void NewController::setNodeSelected(Node *node, bool setSelected)
{
    NodeItem* nodeItem = getNodeItemFromNode(node);

    if(nodeItem == 0){
        qCritical() << "Null NodeItem";
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

            //Unselected
            //qCritical() << node->toString() << "Selected";
            nodeItem->setSelected(true);
            selectedNodes.append(node);
        }

        //Check all selected Edges.
        foreach(Edge* selectedEdge, selectedEdges){
            Node* src = (Node *) selectedEdge->getSource();
            Node* dst = (Node *) selectedEdge->getDestination();

            if(node == src || node == dst){
                setEdgeSelected(selectedEdge, false);
            }
        }


        //Check to see if
    }else{
        nodeItem->setSelected(false);
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
    NodeEdge* nodeEdge = getNodeEdgeFromEdge(edge);

    if(nodeEdge == 0){
        qCritical() << "Null NodeEdge";
        return;
    }

    if(setSelected){

        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        if(!isNodeSelected(src) && !isNodeSelected(dst)){
            if(edge->isInstanceLink()){
                //Select all Children Instance Links between src and dst.
                QVector<Edge*> dstEdges = dst->getEdges(-1);

                foreach(Edge* childEdge, dstEdges){
                    if(childEdge->isInstanceLink()){
                        if(src->isAncestorOf(childEdge->getSource())){
                             NodeEdge* childNodeEdge = getNodeEdgeFromEdge(childEdge);
                             childNodeEdge->setSelected(true);
                             selectedEdges.append(childEdge);
                        }
                    }
                }
            }

            nodeEdge->setSelected(true);
            selectedEdges.append(edge);
        }
    }else{
        nodeEdge->setSelected(false);
        //Remove from list.
        int position = selectedEdges.indexOf(edge);
        selectedEdges.removeAt(position);
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

bool NewController::deleteNode(Node *node)
{
    if(node){
        if(behaviourDefinitions == node){
            behaviourDefinitions = 0;
        }
        if(deploymentDefinitions == node){
            deploymentDefinitions = 0;
        }
        if(interfaceDefinitions == node){
            interfaceDefinitions = 0;
        }

        //Delete any first level children which are instances.

        QVector<Node*> children = node->getChildren();
        QVector<Edge*> childEdges = node->getEdges();

        QString nodesXML = exportGraphML(node);


        if(node->getParentNode()){
            if((node->getParentNode()->isInstance() || node->getParentNode()->isImpl()) && node->getParentNode()->getDefinition() != 0 && node->getDefinition()){
               //Cannot delete Child node.
                qCritical() << "Cannot delete Instance's inheritted child. Must be deleted from Definition.";
                return false;
            }
        }


        //Remove all Edges First.
        foreach(Edge* edge, childEdges){
            Node* src = (Node*) edge->getSource();
            Node* dst = (Node*) edge->getDestination();
            //If this node contains both Edge ends, then it will be contained by the XML exported.
            if(node->isAncestorOf(src) && node->isAncestorOf(dst)){
                deleteEdge(edge, false);
            }else{
                //If it doesn't contain both ends, we need to add an action to reconnect the Edge.
                deleteEdge(edge, true);
            }
        }

        //Remove all Children Nodes
        foreach(Node* child, children){
            Node* childNode = dynamic_cast<Node*>(child);
            if(childNode){
                //Get the NodeItem corresponding to this childNode.
                NodeItem* nodeItem = getNodeItemFromNode(childNode);

                //Remove node from list.
                int nodePosition = nodes.indexOf(childNode);
                int selectedPosition = selectedNodes.indexOf(childNode);
                int nodeItemPosition = nodeItems.indexOf(nodeItem);

                if(nodePosition != -1){
                    nodes.removeAt(nodePosition);
                }
                if(selectedPosition != -1){
                    selectedNodes.removeAt(selectedPosition);
                }
                if(nodeItemPosition != -1){
                    nodeItems.removeAt(nodeItemPosition);
                }
            }
        }

        //Add an action to reverse this action.
        ActionItem action;
        action.actionKind = GraphML::NODE;
        action.actionType = DESTRUCTED;
        action.removedXML = nodesXML;
        action.ID =  node->getID();

        action.parentID = "";

        if(node->getParentNode()){
            action.parentID = node->getParentNode()->getID();
        }

        addActionToStack(action);

        //Get the NodeItem corresponding to this node.
        NodeItem* nodeItem = getNodeItemFromNode(node);


        //Remove node from list.
        int nodePosition = nodes.indexOf(node);
        int selectedPosition = selectedNodes.indexOf(node);
        int nodeItemPosition = nodeItems.indexOf(nodeItem);

        if(nodePosition != -1){
            nodes.removeAt(nodePosition);
        }
        if(selectedPosition != -1){
            selectedNodes.removeAt(selectedPosition);
        }
        if(nodeItemPosition != -1){
            nodeItems.removeAt(nodeItemPosition);
        }

        delete node;
        return true;
    }else{
        qCritical() << "Node doesn't exist!!";
        return false;
    }
}

bool NewController::deleteEdge(Edge *edge, bool addAction)
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
            qCritical() << "Got New Instance Link";
            //Delete Instances/Impls
            teardownInstance(edge->getSource(),edge->getDestination());
        }
        if(edge->isImplLink()){
            qCritical() << "Got New Instance Link";
            //Delete Instances/Impls
            tearDownImpl(edge->getSource(),edge->getDestination());
        }

        if(edge->isImplLink()){
            qCritical() << "Got New Instance Link";
            //Delete Instances/Impls
            tearDownImpl(edge->getSource(),edge->getDestination());
        }

        if(edge->isAggregateLink()){
            qCritical() << "Got New Instance Link";
            //Delete Instances/Impls

            EventPort* eP = dynamic_cast<EventPort*>(edge->getSource());
            if(eP){
                eP->unsetAggregate();
            }
        }




        NodeEdge* nodeEdge = getNodeEdgeFromEdge(edge);

        qCritical() << "Got Node Edge";

        //Remove Edge from list.
        int position = edges.indexOf(edge);
        edges.removeAt(position);

        qCritical() << "Removed From edges";

        //Remove NodeEdge from list.
        position = nodeEdges.indexOf(nodeEdge);
        nodeEdges.removeAt(position);

        qCritical() << "Removed From nodeEdges";

        //Remove Edge from selectedEdges list
        position = selectedEdges.indexOf(edge);
        selectedEdges.removeAt(position);

        qCritical() << "Removed From selectedEdges";

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
    if(src && dst){
        return src->canConnect(dst) && dst->canConnect(src);
    }
    return false;

}

bool NewController::isNodeKindImplemented(QString nodeKind)
{
    return nodeKinds.contains(nodeKind);
}

void NewController::reverseAction(ActionItem action)
{
    switch(action.actionType){

    case CONSTRUCTED:{
        switch(action.actionKind){
        case GraphML::NODE:{
            //Delete Node.
            GraphML* item = getGraphMLFromPreviousID(action.ID);
            Node* node = getNodeFromGraphML(item);
            deleteNode(node);
            break;
        }
        case GraphML::EDGE:{
            GraphML* item = getGraphMLFromPreviousID(action.ID);
            Edge* edge = getEdgeFromGraphML(item);
            deleteEdge(edge, true);
            break;
        }
        case GraphML::DATA:{
            GraphML* item = getGraphMLFromPreviousID(action.ID);
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

            //Get Parent Node
            qCritical() << "Previous Parent: " << action.parentID;

            Node* node = getNodeFromPreviousID(action.parentID);
            view_ImportGraphML(action.removedXML, node, true);
            break;
        }
        case GraphML::EDGE:{
            Node* src = getNodeFromPreviousID(action.srcID);
            Node* dst = getNodeFromPreviousID(action.dstID);

            if(isEdgeLegal(src,dst)){
                view_ConstructEdge(src ,dst, action.dataValues, action.ID);
            }
            break;
        }
        case GraphML::DATA:{
            GraphML* attachedItem = getGraphMLFromPreviousID(action.ID);

            if(attachedItem){
                //Attach GraphML data objects
                attachGraphMLData(attachedItem, action.dataValues);

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

            GraphML* attachedItem = getGraphMLFromPreviousID(action.ID);

            if(attachedItem){
                //Restore the Data Value;
                view_UpdateGraphMLData(attachedItem,action.keyName,action.dataValue);
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
void NewController::attachGraphMLData(GraphML *item, QVector<QStringList> dataList)
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

                ActionItem action;
                action.ID = item->getID();
                action.actionType = CONSTRUCTED;
                action.actionKind = GraphML::DATA;
                action.keyName = keyName;

                if(data){
                    addActionToStack(action);
                    graphMLData.append(data);
                    continue;
                }
            }
        }
        qCritical() << "Data Illegal";
    }
    attachGraphMLData(item, graphMLData);
}

void NewController::attachGraphMLData(GraphML *item, QVector<GraphMLData *> dataList)
{
    if(item){
        item->attachData(dataList);
    }
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
    updateGUIUndoRedoLists();
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

    updateGUIUndoRedoLists();
    emit view_SetGUIEnabled(true);
}

NodeEdge *NewController::getNodeEdgeFromEdge(Edge *edge)
{
    foreach(NodeEdge* nodeEdge, nodeEdges){
        if(nodeEdge->getGraphML() == edge){
            return nodeEdge;
        }
    }
    return 0;
}

void NewController::deleteSelectedNodes()
{
    int size = selectedNodes.size();

    emit view_SetSelectedAttributeModel(0);
    while(selectedNodes.size() > 0){
        Node* node = selectedNodes.front();
        if(node && node->isAncestorOf(centeredNode)){
            centeredNode = 0;
            qCritical() << "Unsetting Cenetered Node";
        }
        bool deleted = deleteNode(node);


        if(!deleted){
            NodeItem* nodeItem = getNodeItemFromNode(node);
            if(nodeItem){
                nodeItem->setSelected(false);
                selectedNodes.removeFirst();
            }
        }
    }
}

void NewController::deleteSelectedEdges()
{
    foreach(Edge* edge, selectedEdges){
        deleteEdge(edge);
    }
    selectedEdges.clear();
}

void NewController::setupNode(Node *node)
{
    if(!node){
        qCritical() << "Null node";
    }
    //Construct Action

    ActionItem action;
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::NODE;
    action.ID = node->getID();

    //Get the Parent ID.
    QString parentID = "";
    if(node->getParentNode()){
        parentID = node->getParentNode()->getID();
    }

    action.parentID = parentID;

    addActionToStack(action);

    nodes.append(node);


    view_ConstructNodeItem(node);
}

void NewController::setupModel()
{
    GraphMLKey* label = constructGraphMLKey("label", "string", "node");
    GraphMLKey* k = constructGraphMLKey("kind", "string", "node");

    QVector<GraphMLData *> data;

    data.append(new GraphMLData(k, "Model"));

    GraphMLData* labelD = new GraphMLData(label, "New Project");

    data.append(labelD);
    connect(labelD, SIGNAL(dataChanged(GraphMLData*)), this, SLOT(view_ProjectNameUpdated(GraphMLData*)));

    model = (Model*)constructGraphMLNode(data, 0);



    constructNode(model,"BehaviourDefinitions",QPointF(0,0));
    constructNode(model,"InterfaceDefinitions",QPointF(4100,0));
    constructNode(model,"DeploymentDefinitions",QPointF(8200,0));

    behaviourDefinitions->updateDataValue("label", "Behaviour Definitions");
    interfaceDefinitions->updateDataValue("label", "Interface Definitions");
    deploymentDefinitions->updateDataValue("label", "Deployment Definitions");
}

void NewController::setupInstance(Node *definition, Node *instance)
{
    if(!instance || !definition){
        return;
    }


    foreach(GraphMLData* attachedData, definition->getData()){

        if(!attachedData->getProtected()){
            GraphMLData* newData = instance->getData(attachedData->getKey());

            if(!newData){
                newData = new GraphMLData(attachedData->getKey(), attachedData->getValue());
                instance->attachData(newData);
                newData->setProtected(true);
            }
            if(newData){
                attachedData->bindData(newData);
            }
        }
    }

    foreach(Node* child, definition->getChildren(0)){
        if(child->isDefinition()){
            constructNodeInstance(instance, child);
        }
    }

    //Specific
    definition->addInstance(instance);
}

void NewController::setupImpl(Node *definition, Node *impl)
{
    if(!impl || !definition){
        return;
    }


    foreach(GraphMLData* attachedData, definition->getData()){

        if(!attachedData->getProtected()){
            GraphMLData* newData = impl->getData(attachedData->getKey());

            if(!newData){
                newData = new GraphMLData(attachedData->getKey(), attachedData->getValue());
                impl->attachData(newData);
                newData->setProtected(true);
            }
            if(newData){
                attachedData->bindData(newData);
            }
        }
    }

    foreach(Node* child, definition->getChildren(0)){
        if(child->isDefinition()){
            constructNodeImpl(impl, child);
        }
    }

    //Specific
    definition->setImplementation(impl);
}

void NewController::tearDownImpl(Node *definition, Node *implementation)
{
    qCritical() << "Tearing Down Impl: " << definition->toString() << " <-> " << implementation->toString();
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
        definition->unsetImplementation();
        if(!selectedNodes.contains(implementation)){
            selectedNodes.push_front(implementation);
        }
    }

}

void NewController::teardownInstance(Node *definition, Node *instance)
{
    qCritical() << "Tearing Down Instance: " << definition->toString() << " <-> " << instance->toString();
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
            qCritical() << "Selected Nodes[0] = " << instance->toString();
        }
    }

    qCritical() << "Done Tearing Donw";
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
            qCritical() << "NewController::isGraphMLValid() << Parsing Error! Line Number: " << lineNumber;
            qCritical() << "\t" << xmlErrorChecking.errorString();
            //qCritical() << inputGraphML;
            return false;
        }
    }
    return true;
}

void NewController::setupEdge(Edge *edge)
{
    ActionItem action;
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::EDGE;
    action.ID = edge->getID();


    addActionToStack(action);

    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    if(src->isDefinition() && (dst->isInstance() || dst->isImpl())){
        //Check the types.
        QString srcKind = src->getDataValue("kind");
        QString dstKind = dst->getDataValue("kind");

        if(srcKind + "Instance" == dstKind){
            setupInstance(src, dst);
        }else if(srcKind + "Impl" == dstKind){
            setupImpl(src, dst);
        }
    }

    if(edge->isAggregateLink()){
        EventPort* eP = dynamic_cast<EventPort*>(src);
        if(eP){
            qCritical() << "Setting Aggregate";
            eP->setAggregate(dst);
        }
    }

    //Add the edge to the list of edges constructed.
    edges.append(edge);
    view_ConstructNodeEdge(edge);
}

void NewController::updateGUIUndoRedoLists()
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

NodeItem *NewController::getNodeItemFromNode(Node *node)
{
    foreach(NodeItem* nodeItem, nodeItems){
        if(nodeItem->getGraphML() == node){
            return nodeItem;
        }
    }
    return 0;
}

GraphML *NewController::getGraphMLFromID(QString ID)
{
    Node* node = getNodeFromID(ID);
    Edge* edge = getEdgeFromID(ID);

    if(node){
        return node;
    }
    if(edge){
        return edge;
    }
    qCritical() << "ID: " << ID << " Not found!";
    return 0;
}

Node *NewController::getNodeFromID(QString ID)
{
    foreach(Node* node, nodes){
        if(node->getID() == ID){
            return node;
        }
    }
    return 0;
}

Edge *NewController::getEdgeFromID(QString ID)
{
    foreach(Edge* edge, edges){
        if(edge->getID() == ID){
            return edge;
        }
    }
    return 0;

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
         if(pastIDLookup.contains(ID)){
             QString temp = ID;
             ID = newID;
             newID = pastIDLookup[temp];
         }else{
             break;
         }
    }

    if(ID != originalID){
        qDebug() << "Looking for ID: " <<originalID << " Found ID:" << ID;
    }
    return ID;

}

GraphML *NewController::getGraphMLFromPreviousID(QString ID)
{
    ID = getNewIDFromPreviousID(ID);
    return getGraphMLFromID(ID);
}

void NewController::linkPreviousIDToID(QString previousID, QString newID)
{
    pastIDLookup[previousID] = newID;
    if(!pastIDLookup.contains(newID)){
        pastIDLookup[newID] = "";
    }
    qCritical() << "Linked ID:" << previousID << " to ID:" << newID;

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


