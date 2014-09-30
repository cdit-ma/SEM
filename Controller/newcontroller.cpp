#include "newcontroller.h"

#include <QDebug>



NewController::NewController(NodeView *v)
{
    UNDOING = false;
    HIDDEN_OPACITY = 0.10;

    //Attach the view.
    view = v;
    model = new Model();

    currentActionID = 0;
    actionCount = 0;
    currentAction = "";

    childNodeKind="";

    centeredNode = 0;
    nodeKinds << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "InEventPortIDL"  << "OutEventPort" << "OutEventPortIDL" << "Attribute" << "HardwareNode" << "HardwareCluster" << "PeriodicEvent" << "Component" << "Member";


    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftPressed(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeletePressed(bool)));
    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(unselect()),this, SLOT(view_ClearSelection()));
    connect(view, SIGNAL(constructNodeItem(QPointF)), this,SLOT(view_ConstructChildNode(QPointF)));
    connect(this, SIGNAL(view_SetNodeItemCentered(NodeItem*)), view, SLOT(centreItem(NodeItem*)));

    connect(this, SIGNAL(view_SetRubberbandSelectionMode(bool)), view, SLOT(setRubberBandMode(bool)));

    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;

    viewAspects << "Assembly" << "Workload";
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
        foreach(GraphMLContainer* child, node->getChildren()){
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

QStringList NewController::getNodeKinds()
{
    return nodeKinds;
}

QStringList NewController::getViewAspects()
{
    return viewAspects;
}

void NewController::view_ImportGraphML(QStringList inputGraphML, GraphMLContainer *currentParent)
{
    view_ActionTriggered("Importing GraphML");
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

   // emit view_UpdateProgressDialog(false);
   // emit view_EnableGUI(true);

}

void NewController::view_ImportGraphML(QString inputGraphML, GraphMLContainer *currentParent)
{
    qCritical() << "NewController::view_ImportGraphML()";

    //Key Lookup provides a way for the original key "id" to be linked with the internal object GraphMLKey
    QMap<QString , GraphMLKey*> keyLookup;

    //Node lookup provides a way for the original edge source/target ID's to be linked with the internal object GraphMLContainer
    QMap<QString, GraphMLContainer *> nodeLookup;

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
        qCritical() << "Using Parent Graph";
        currentParent = model->getGraph();
    }

    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xmlErrorChecking(inputGraphML);

    //Check for Errors
    while(!xmlErrorChecking.atEnd()){
        xmlErrorChecking.readNext();
        float lineNumber = xmlErrorChecking.lineNumber();
        if (xmlErrorChecking.hasError()){
            qCritical() << "Model::importGraphML() << Parsing Error! Line Number: " << lineNumber;
            qCritical() << "\t" << xmlErrorChecking.errorString();
            qCritical() << inputGraphML;
        }
    }
    qCritical() << "No Errors";

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(inputGraphML);

    //Get the number of lines in the input GraphML XML String.
    float lineCount = inputGraphML.count("\n") / 100;


    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;

    //While the document has more lines.
    while(!xml.atEnd()){
        //Read the next tag
        xml.readNext();

        //Calculate the current percentage
        float lineNumber = xml.lineNumber();
        double percentage = (lineNumber * 100 / lineCount);
        //emit view_UpdateProgressDialog(percentage);

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
                    currentParent = currentParent->getParent();

                    //Check if the parent is a graph or a node!
                    //We only care about if it's not a graph!
                    Graph* graph = dynamic_cast<Graph*> (currentParent);
                    if(graph != 0){
                        parentDepth --;
                    }
                }

                //Add the new Node to the lookup table.
                nodeLookup.insert(nodeID, newNode);

                if(UNDOING || REDOING){
                    qCritical() << "Linking " << nodeID << " TO " << newNode->getID();
                    previousNodeIDLookup[nodeID] = newNode->getID();
                    previousNodeIDLookup[newNode->getID()] = "";

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
        GraphMLContainer* s = nodeLookup[edge.source];
        GraphMLContainer* d = nodeLookup[edge.target];

        if(s != 0 && d != 0){
            if(s->isEdgeLegal(d) && d->isEdgeLegal(s)){
                //Construct the edge, and attach the data.
                Edge* newEdge = new Edge(s, d);
                newEdge->attachData(edge.data);
                setupEdge(newEdge);
            }else{
                qCritical() << QString("Line #%1: Edge Not Valid!").arg(QString::number(edge.lineNumber));
                //return false;
            }
        }else{
            qCritical() << "Edge is Illegal!";
            //return false;
        }
    }

    qCritical() << "Import finished?";

}

void NewController::view_ExportGraphML()
{

    QVector<Node*> nodes;

    foreach(GraphMLContainer* child, model->getGraph()->getChildren(0)){
        Node* node = dynamic_cast<Node*>(child);
        if(node){
            nodes.append(node);
        }
    }

    QString data = exportGraphML(nodes);

    emit view_ExportGraphML(data);
}

void NewController::view_SetNodeSelected(Node *node, bool setSelected)
{
    qCritical() << "view_SetNodeSelected";

    if(KEY_CONTROL_DOWN){
        //If it contains node, unselect it.
        bool setSelected = !isNodeSelected(node);
        setNodeSelected(node, setSelected);
    }else{
        if(KEY_SHIFT_DOWN){
            Node* src = getSelectedNode();
            if(selectedEdges.size() == 0 && src != 0){
                view_ActionTriggered("Constructing Child Node");
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

    //Get the current Selected Node.
    GraphMLContainer* parent = getSelectedNode();
    if(!parent){
        parent = model->getGraph();
    }

    GraphMLKey* x = constructGraphMLKey("x", "double", "node");
    GraphMLKey* y = constructGraphMLKey("y", "double", "node");


    GraphMLKey* k = constructGraphMLKey("kind", "string", "node");
    GraphMLKey* t = constructGraphMLKey("type", "string", "node");
    GraphMLKey* l = constructGraphMLKey("label", "string", "node");

    QVector<GraphMLData *> data;

    data.append(new GraphMLData(x, QString::number(centerPoint.x())));
    data.append(new GraphMLData(y, QString::number(centerPoint.y())));
    data.append(new GraphMLData(k, childNodeKind));
    data.append(new GraphMLData(t, childNodeKind));
    data.append(new GraphMLData(l, "new_" + childNodeKind ));

    view_ActionTriggered("Constructing Child Node");
    constructGraphMLNode(data, parent);

}

void NewController::view_ConstructEdge(Node *src, Node *dst)
{
    if(isEdgeLegal(src, dst)){
        Edge* edge = new Edge(src, dst);
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
        xData->setValue(QString::number(x));
        yData->setValue(QString::number(y));
    }

}

void NewController::view_SetChildNodeKind(QString nodeKind)
{

    childNodeKind = nodeKind;
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

void NewController::view_ControlPressed(bool isDown)
{

    qCritical() << "Undo States: " << undoStack.count();
    /*
    qCritical() << "Edges: " << edges.count();
    qCritical() << "Nodes: " << nodes.count();
    qCritical() << "NodeItems: " << nodeItems.count();
    qCritical() << "NodeEdges: " << nodeEdges.count();
    qCritical() << "SelectedNodes: " << selectedNodes.count();
    qCritical() << "SelectedEdges: " << selectedEdges.count();
*/

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
     if(isDown){
         view_ActionTriggered("Deleting Selection");
         //Do Delete
         deleteSelectedEdges();
         deleteSelectedNodes();
     }
}

void NewController::view_Undo()
{
    UNDOING = true;
    undoRedo();
    /*if(undoStack.size() > 0){
        invertAction(undoStack.pop());
    }*/
    UNDOING = false;
    view->scene()->update();
}

void NewController::view_Redo()
{
    REDOING = true;
    undoRedo();
    REDOING = false;
    view->scene()->update();
}

void NewController::view_SelectAll()
{
    qCritical() << "NewController::view_SelectAll";

    clearSelectedNodes();

    if(centeredNode){
        //Get children.
        foreach(GraphMLContainer* child, centeredNode->getChildren(2)){
            Node* node = dynamic_cast<Node*>(child);
            if(node){
                setNodeSelected(node);
            }
        }
    }else{
        //Get children.
        foreach(GraphMLContainer* child, model->getGraph()->getChildren(0)){
            Node* node = dynamic_cast<Node*>(child);
            if(node){
                setNodeSelected(node);
            }
        }
    }

}

void NewController::view_ClearSelection()
{
    clearSelectedEdges();
    clearSelectedNodes();

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

        if(!parentNodeItem){
            qCritical() << "No NodeItem";
        }
        NodeItem* nodeItem = new NodeItem(node, parentNodeItem);

        nodeItems.append(nodeItem);
        view->addNodeItem(nodeItem);

        connect(nodeItem, SIGNAL(setNodeSelected(Node*, bool)),this, SLOT(view_SetNodeSelected(Node*,bool)));
        connect(nodeItem, SIGNAL(centreNode(Node*)), this, SLOT(view_SetNodeCentered(Node*)));
        connect(nodeItem, SIGNAL(makeChildNode(QPointF)), this,SLOT(view_ConstructChildNode(QPointF)));
        connect(nodeItem, SIGNAL(moveSelection(QPointF)), this, SLOT(view_MoveSelectedNodes(QPointF)));

        connect(node, SIGNAL(destroyed()), nodeItem, SLOT(destructNodeItem()));
        connect(this, SIGNAL(view_SetRubberbandSelectionMode(bool)),nodeItem, SLOT(setRubberbandMode(bool)));

        connect(view, SIGNAL(controlPressed(bool)), nodeItem, SLOT(controlPressed(bool)));
        /*
        QString itemID = node->getID();
        QString parentItemID = "";
        if(parentNode != 0){
            parentItemID = parentNode->getID();
            //qCritical() << parentNode->getID();

        }else{
           // qCritical() << "No Parent yet?!";
        }
        */

        //Insert an action to reverse it.
/*
        Action newNode;
        newNode.actionType = CONSTRUCT;
        newNode.itemKind = item->getKind();
        newNode.itemID = node->getID();
        newNode.srcID = "";
        newNode.dstID = "";
        newNode.removedXML = "";

        addActionToStack(newNode);

        undoLookup[newNode.itemID] = node;

        if(!undoIDStack.isEmpty()){
            QString oldID = undoIDStack.pop();
            qCritical() << "Linking old ID: " << oldID << " to: " << node->toString();
            undoLookup[oldID] = node;
        }
        connect(newNodeItem, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(nodeItem_Selected(NodeItem*)));
        connect(newNodeItem, SIGNAL(centreNode(NodeItem*)),this, SLOT(nodeItem_SetCentered(NodeItem*)));
        connect(newNodeItem, SIGNAL(makeChildNode(QPointF, Node*)), this, SLOT(nodeItem_MakeChildNode(QPointF, Node*)));

        connect(newNodeItem, SIGNAL(actionTriggered(QString)), this, SLOT(view_ActionTriggered(QString)));
        connect(newNodeItem, SIGNAL(updateGraphMLData(Node*,QString,QString)), this, SLOT(view_updateGraphMLData(Node*,QString,QString)));
        //Add item to the Tree Model.


        QStandardItem* newNodeItemData = new QStandardItem(node->getDataValue("label"));
        newNodeItemData->setFlags(newNodeItemData->flags() & ~Qt::ItemIsEditable);


        GUIContainer* toAppend = new GUIContainer();

        toAppend->node = node;
        toAppend->nodeItem = newNodeItem;
        toAppend->modelItem = newNodeItemData;
        //toAppend->deleteXML = node->toGraphML();

        nodeContainers.append(toAppend);

        GUIContainer* parent = getGUIContainer(parentNode);


        if(parent == 0){
            treeModel->appendRow(newNodeItemData);
        }else{
            parent->modelItem->appendRow(newNodeItemData);


             //NodeItemTreeItem* parentItem = parentNodeItem->getTreeModelItem();
            // parentItem->appendChild(modelItem);


            //parentNodeItem->getTreeModelItem()->appendChild(newNodeItem->getTreeModelItem());
        }

         */

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
        connect(nodeEdge, SIGNAL(setSelected(Edge*,bool)), this, SLOT(view_SetEdgeSelected(Edge*,bool)));

        view->addEdgeItem(nodeEdge);
    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }
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

Node *NewController::constructGraphMLNode(QVector<GraphMLData *> data, GraphMLContainer *parent)
{
    Node *newNode;

    GraphMLKey* w = constructGraphMLKey("width", "double", "node");
    GraphMLKey* h = constructGraphMLKey("height", "double", "node");

    bool noWidth = true;
    bool noHeight = true;
    QString kind;
    //Get kind from nodeData.
    for(int i=0; i < data.size(); i++){
        if(data[i]->getKey()->getName() == "kind"){
            kind = data[i]->getValue();
        }
        if(data[i]->getKey() == w){
            noWidth = false;
        }
        if(data[i]->getKey() == h){
            noHeight = false;
        }
    }

    if(noWidth){
        data.append(new GraphMLData(w, ""));
    }
    if(noHeight){
        data.append(new GraphMLData(h, ""));
    }


    QVector<QString> aspects;
    if(kind == "ComponentAssembly"){
        newNode = new ComponentAssembly();
        aspects << "Assembly" << "Workload";
    }else if(kind == "ComponentInstance"){
        newNode = new ComponentInstance();
        aspects << "Assembly" << "Workload";
    }else if(kind == "Attribute"){
        newNode = new Attribute();
        aspects << "Assembly";
    }else if(kind == "OutEventPortIDL"){
        newNode = new OutEventPortIDL();
        aspects << "Assembly" << "Workload";
    }else if(kind == "InEventPortIDL"){
        newNode = new InEventPortIDL();
        aspects << "Assembly" << "Workload";
    }else if(kind == "HardwareNode"){
        newNode = new HardwareNode();
        aspects << "Assembly";
    }else if(kind == "HardwareCluster"){
        newNode = new HardwareCluster();
        aspects << "Assembly";
    }else if(kind == "PeriodicEvent"){
        newNode = new PeriodicEvent();
        aspects << "Workload" ;
    }else if(kind == "Component"){
        newNode = new Component();
        aspects << "Workload";
    }else if(kind == "InEventPort"){
        newNode = new InEventPort();
        aspects << "Workload";
    }else if(kind == "Member"){
        newNode = new Member();
        aspects << "Workload" << "Assembly";
    }

    else{
        qCritical() << "Kind:" << kind << "Not implemented";
        newNode = new BlankNode();
        aspects << "Workload" << "Assembly";
    }

    foreach(QString aspect, aspects){
        newNode->addAspect(aspect);
    }

    newNode->attachData(data);
    qCritical() << "Node Made!!?";
    //Adopt the new Node into the parent
    if(parent->isAdoptLegal(newNode)){
        parent->adopt(newNode);
        setupNode(newNode);
    }else{
        qCritical() << "Cannot adopt";
        //Delete the newly created node.
        delete newNode;
        return 0;
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

    keys.append(attribute);
    return attribute;
}



void NewController::clearSelectedNodes()
{
    qCritical() << "NewController::clearSelectedNodes";
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
    qCritical() << "NewController::clearSelectedEdges";
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
            nodeItem->setSelected(true);
            selectedNodes.append(node);
            qCritical() << "Selected: " << node->getID();
            qCritical() << "Size: " << this->selectedNodes.size();
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

        Node* src = (Node *) edge->getSource();
        Node* dst = (Node *) edge->getDestination();

        if(!isNodeSelected(src) && !isNodeSelected(dst)){
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

void NewController::deleteNode(Node *node, bool addAction)
{
    if(node){
        QVector<GraphMLContainer*> children = node->getChildren(1);
        QVector<Edge*> childEdges = node->getEdges(1);

        QString xml;
        if(addAction){
            xml = exportGraphML(node);
        }


        //Remove all Children Nodes
        foreach(GraphMLContainer* child, children){
            Node* childNode = dynamic_cast<Node*>(child);
            if(childNode){
                deleteNode(childNode, false);
            }
        }

        //Remove all Edges
        foreach(Edge* edge, childEdges){
            deleteEdge(edge, true);
        }

        NodeItem* nodeItem = getNodeItemFromNode(node);

        //Remove Node from list.
        int position = nodes.indexOf(node);
        nodes.removeAt(position);

        //Remove NodeItem from list.
        position = nodeItems.indexOf(nodeItem);
        nodeItems.removeAt(position);

        //Remove Node from selectedNode list
        //position = selectedNodes.indexOf(childNode);
        //selectedNodes.removeAt(position);

        if(addAction){
            ActionItem action;
            action.actionKind = GraphML::NODE;
            action.actionType = DESTRUCTED;
            action.removedXML = xml;
            action.ID = node->getID();

            action.parentID = "";

            if(node->getParentNode()){
                action.parentID = node->getParentNode()->getID();
            }
            addActionToStack(action);
        }



        delete node;
    }else{
        qCritical() << "Node doesn't exist!!";
    }
}

void NewController::deleteEdge(Edge *edge, bool addAction)
{
    if(edge){
        if(addAction){
            ActionItem action;
            action.actionType = DESTRUCTED;
            action.actionKind = GraphML::EDGE;
            action.ID = edge->getID();

            action.srcID = edge->getSource()->getID();
            action.dstID = edge->getDestination()->getID();

            addActionToStack(action);
        }


        NodeEdge* nodeEdge = getNodeEdgeFromEdge(edge);

        //Remove Edge from list.
        int position = edges.indexOf(edge);
        edges.removeAt(position);

        //Remove NodeEdge from list.
        position = nodeEdges.indexOf(nodeEdge);
        nodeEdges.removeAt(position);

        //Remove Edge from selectedEdges list
        position = selectedEdges.indexOf(edge);
        selectedEdges.removeAt(position);

        delete edge;
    }else{
         qCritical() << "Edge doesn't exist!!";

    }
}

bool NewController::isNodesAncestorSelected(Node *selectedNode)
{
    foreach(Node* node, selectedNodes){
        if(node->isAncestorOf(selectedNode)){
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
        return src->isEdgeLegal(dst) && dst->isEdgeLegal(src);
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
            Node* node = getNodeFromPreviousID(action.ID);
            deleteNode(node);
            break;
        }
        case GraphML::EDGE:{
            Edge* edge = getEdgeFromID(action.ID);
            deleteEdge(edge);
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
            view_ImportGraphML(action.removedXML, node);
            break;
        }
        case GraphML::EDGE:{
            Node* src = getNodeFromPreviousID(action.srcID);
            Node* dst = getNodeFromPreviousID(action.dstID);
            qCritical() << src->toString();
            qCritical() << dst->toString();
            if(isEdgeLegal(src,dst)){
                view_ConstructEdge(src,dst);
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

void NewController::addActionToStack(ActionItem action)
{
    //Get Current Action ID and action.
    action.actionID = currentActionID;
    action.actionName = currentAction;

    if(UNDOING){
        qCritical() << "Added to Redo Stack";
        redoStack.push(action);
    }else{
        qCritical() << "Added to Undo Stack";
        undoStack.push(action);
    }
}

void NewController::undoRedo()
{
    QStack<ActionItem> reverseStack;

    if(UNDOING){
        reverseStack = undoStack;
    }else{
        reverseStack = redoStack;
    }

    if(reverseStack.size() == 0){
        qCritical () << "No Actions to reverse!";
        return;
    }

    int actionID = reverseStack.top().actionID;
    QString actionName = reverseStack.top().actionName;

    //Emit a new action so this Undo operation can itself be undone.
    view_ActionTriggered(actionName);
    //emit view_ActionTriggered(actionName);

    qCritical() << "Current Action: " << actionID;
    //Reverse all the actions which match the actionID.

    while(reverseStack.size() > 0){
        ActionItem action = reverseStack.top();

        qCritical() << "Action: " << action.actionID;
        //If this action is part of the currentAction reverse it.
        if(action.actionID == actionID){
            reverseAction(reverseStack.pop());
        }else{
            break;
        }
    }

    if(UNDOING){
        undoStack = reverseStack;
    }else{
        redoStack = reverseStack;
    }

}

NodeEdge *NewController::getNodeEdgeFromEdge(Edge *edge)
{
    foreach(NodeEdge* nodeEdge, nodeEdges){
        if(nodeEdge->edge == edge){
            return nodeEdge;
        }
    }
    return 0;
}

void NewController::deleteSelectedNodes()
{
    foreach(Node* node, selectedNodes){
        if(node->isAncestorOf(centeredNode)){
            centeredNode = 0;
        }

        deleteNode(node);
    }
    selectedNodes.clear();
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

void NewController::setupEdge(Edge *edge)
{
    ActionItem action;
    action.actionType = CONSTRUCTED;
    action.actionKind = GraphML::EDGE;
    action.ID = edge->getID();


    addActionToStack(action);

    //Add the edge to the list of edges constructed.
    edges.append(edge);
    view_ConstructNodeEdge(edge);
}

NodeItem *NewController::getNodeItemFromNode(Node *node)
{
    foreach(NodeItem* nodeItem, nodeItems){
        if(nodeItem->node == node){
            return nodeItem;
        }
    }
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
    qCritical() << "OLD ID:" << ID;
    QString newID = ID;
    while(newID != ""){
         if(previousNodeIDLookup.contains(ID)){
             QString temp = ID;
             ID = newID;
             newID = previousNodeIDLookup[temp];
         }else{
             break;
         }
    }
    qCritical() << "NEW ID:" << ID;

    Node* node = getNodeFromID(ID);


    if(node){
    }else{
        qCritical() << "No Node!";
    }
    return node;
}

