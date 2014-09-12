#include "graphmlcontroller.h"
#include "../GUI/nodeconnection.h"
#include <QDebug>

//Constructor
GraphMLController::GraphMLController(NodeView* view):QObject()
{
    this->model = new Model();
    this->view = view;
   // this->model = model;

    UNDOING = false;
    REDOING = false;

    //Setup variables;
    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;

    //Connect View and Model
    currentMaximized=0;

    currentAction = "";
    currentActionID = 0;
    actionCount = 0;

    treeModel = new QStandardItemModel(this);


    //Connect the Signals to the VIEW
    connect(this, SIGNAL(view_addNodeEdge(NodeEdge*)),view,SLOT(addEdgeItem(NodeEdge*)));
    connect(this, SIGNAL(view_addNodeItem(NodeItem*)),view,SLOT(addNodeItem(NodeItem*)));
    connect(this, SIGNAL(view_centerNodeItem(NodeItem*)),view,SLOT(centreItem(NodeItem*)));
    connect(this, SIGNAL(model_ConstructNode(QString, GraphMLContainer*)), model, SLOT(view_ConstructNode(QString, GraphMLContainer*)));

    connect(this, SIGNAL(model_ConstructEdge(GraphMLContainer*, GraphMLContainer*)), model, SLOT(view_ConstructEdge(GraphMLContainer*, GraphMLContainer*)));


    //Connect to the Signals from the VIEW
    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftTriggered(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeleteTriggered(bool)));
    connect(view, SIGNAL(unselect()),this, SLOT(view_EmptyScenePressed()));
    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(copy()), this, SLOT(view_Copy()));
    connect(view, SIGNAL(cut()), this, SLOT(view_Cut()));
    connect(view, SIGNAL(undo()), this, SLOT(view_Undo()));
    connect(view, SIGNAL(redo()), this, SLOT(view_Redo()));

    //Connect the Signals to the MODEL
    connect(this, SIGNAL(model_ExportGraphML(QString)), model, SLOT(view_ExportGraphML(QString)));
    connect(this, SIGNAL(model_ImportGraphML(QStringList, GraphMLContainer*)), model, SLOT(view_ImportGraphML(QStringList, GraphMLContainer*)));
    connect(model, SIGNAL(view_EnableGUI(bool)), this, SLOT(model_EnableGUI(bool)));

    connect(model, SIGNAL(controller_ActionTrigger(QString)), this, SLOT(view_ActionTriggered(QString)));
    connect(model, SIGNAL(view_DestructGUINode(GraphMLContainer*, QString)), this, SLOT(model_RemoveNode(GraphMLContainer*, QString)));
    connect(model, SIGNAL(view_DestructGUIEdge(Edge*, QString, QString)), this, SLOT(model_RemoveEdge(Edge*, QString, QString)));

    //connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)),this, SLOT(deleteComponent(GraphMLContainer*)));


    //Connect to the Signals of the MODEL
    connect(model, SIGNAL(view_ConstructGUINode(GraphMLContainer*)), this,SLOT(model_MadeNode(GraphMLContainer*)));
    //connect(model,SIGNAL(constructNodeItem(Node*)),this,SLOT(model_MakeNode(Node*)));
    connect(model,SIGNAL(view_ConstructGUIEdge(Edge*)),this,SLOT(model_MadeEdge(Edge*)));


    connect(model, SIGNAL(controller_FinishedAction()), this, SLOT(modelActionComplete()));


    connect(model,SIGNAL(view_ReturnExportedData(QString,QString)), this, SLOT(model_WriteToFile(QString,QString)));

}

GraphMLController::~GraphMLController()
{
    delete model;
}

Model *GraphMLController::getModel()
{
    return model;

}

QStandardItemModel *GraphMLController::getTreeModel()
{
    return this->treeModel;
}

void GraphMLController::modelActionComplete()
{
    qCritical() << "Action Completed.";

    qCritical() << "ACTIONS LEFT" << reversingStack.size();
    if(reversingStack.size() > 0){
        Action action = reversingStack.pop();
        reverseAction(action);
    }
    //Continue on.
}

void GraphMLController::view_Copy()
{

    QVector<GraphMLContainer*> nodes;

    for(int i=0; i < selectedNodeItems.size(); i++){
        nodes.append(selectedNodeItems[i]->node);
    }


    QString result = model->exportGraphML(nodes);

    emit view_CopyText(result);
}

void GraphMLController::view_Cut()
{
    view_Copy();

    deleteSelectedNodeItems();
}

void GraphMLController::view_Paste(QString XMLData)
{
    QStringList files;
    files << XMLData;

    GraphMLContainer* currentParent = 0;
    if(selectedNodeItems.size() == 1){
        currentParent = selectedNodeItems[0]->node;
    }

    emit model_ImportGraphML(files, currentParent);
}


void GraphMLController::view_ImportGraphML(QStringList inputGraphML)
{
    qCritical() << "GraphMLController::view_ImportGraphML";
    emit model_ImportGraphML(inputGraphML);
}

void GraphMLController::view_ExportGraphML(QString filePath)
{
    if(filePath != ""){
        qCritical() << "GraphMLController::view_ExportGraphML";
        emit model_ExportGraphML(filePath);
    }
}

void GraphMLController::view_UpdateLabel(QString value)
{
    if(selectedNodeItems.size() == 1){
        view_ActionTriggered("Updating Label");
        view_updateGraphMLData(selectedNodeItems[0]->node, "label", value);
        //selectedNodeItems[0]->node->updateDataValue("label", value);
        //GUIContainer* gi = getGUIContainer(selectedNodeItems[0]);
        //gi->modelItem->setText(value);
    }
}


bool GraphMLController::reverseAction(Action action)
{
    qCritical() << "Reversing Action: ";
    if(action.actionType == CONSTRUCT){
            QString ID = action.itemID;

            GraphML* item = undoLookup[ID];
            if(item != 0){
                removeGraphML(item);
            }
    }else if(action.actionType == DESTRUCT){
        if(action.itemKind == GraphML::NODE){

            QString parentID = action.parentID;

            GraphMLContainer* parent;
            if(parentID == ""){

                //Use Graph.
                parent = 0;
            }else{
                qCritical () << "Looking for ID: " << parentID;
                parent =  (GraphMLContainer*)undoLookup[parentID];
            }

            QStringList xmlData;

            xmlData << action.removedXML;

            if(action.removedXML != ""){
                //model->importGraphML(action.removedXML, parent);
                emit model_ImportGraphML(xmlData, parent);
            }else{
                qCritical() << action.itemID;
                qCritical() << "NO XML!";
            }

        }else if (action.itemKind == GraphML::EDGE){
            qCritical() << "Edge";
            QString srcID = action.srcID;
            QString dstID = action.dstID;

            qCritical() << "Edge: " << srcID << " <-> " << dstID;

            GraphMLContainer* src = (GraphMLContainer*)undoLookup[srcID];
            GraphMLContainer* dst = (GraphMLContainer*)undoLookup[dstID];
            qCritical() << "Edge: " << src->getID() << " <-> " << dst->getID();

            if(src == 0 || dst == 0){
                qCritical() << "NULLAGE";
                return false;

            }

            qCritical() << "Making Edge between src and dst";

            emit model_ConstructEdge(src, dst);
        }else{
            qCritical() << "UNKNOWN ACTION KIND";
            qCritical() << action.itemKind;
        }

    }else if(action.actionType == CHANGED){
        QString itemID = action.itemID;
        GraphMLContainer* item = (GraphMLContainer*)undoLookup[itemID];

        if(item != 0){
            emit view_updateGraphMLData((Node*)item,action.key, action.previousValue);
        }
    }


    else{
            qCritical() << "UNKNOWN ACTION TYPE";
    }

    qCritical() << "FINISHED REVERSING!";

    return true;
}

void GraphMLController::removeGraphML(GraphML *graph)
{
    GraphMLContainer* node = dynamic_cast<GraphMLContainer*>(graph);

    if(node != 0){
        GUIContainer* item = getGUIContainer((Node*)node);

        if(item != 0){
            QVector<GraphMLContainer*> nodes;
            nodes.append(node);

            item->deleteXML = model->exportGraphML(nodes);
            Node* parentNode = ((Node*)node)->getParentNode();
            if(parentNode != 0){
                item->parentNodeID = parentNode->getID();
            }else{
                item->parentNodeID = "";
            }
        }
        delete graph;
    }
}


void GraphMLController::view_Undo()
{
    UNDOING = true;

    int cActionID = currentActionID;
    QString cActionName = currentAction;

    if(undoStack.size() > 0){
        emit view_ActionTriggered("Undoing: " + cActionName);

        qCritical() << "cActionName["<<cActionID<<"]: " << cActionName;

        reversingStack.clear();
        // reverseAction(action);


        while(undoStack.size() > 0){
            Action action = undoStack.pop();
            int actionID = action.actionID;

            qCritical() << actionID;
            if(actionID == cActionID){
                reverseAction(action);
                //reversingStack.push_front(action);
            }else{
                currentActionID = actionID;
                currentAction = action.actionName;
                undoStack.append(action);
                break;
            }
        }
    }
}

void GraphMLController::view_Redo()
{
    int cActionID = currentActionID;
    QString cActionName = currentAction;

    UNDOING = false;
    emit view_ActionTriggered("Redoing: " + cActionName);

    qCritical() << "cActionName["<<cActionID<<"]: " << cActionName;


    while(redoStack.size() > 0){
        Action action = redoStack.pop();
        int actionID = action.actionID;

        qCritical() << actionID;
        if(actionID == cActionID){
            reverseAction(action);
        }else{
            qCritical() << "FOund new Action";
            currentActionID = actionID;
            currentAction = action.actionName;
            redoStack.append(action);
            break;
        }
    }

/*
    if(redoStack.size() > 0){
        reverseAction(redoStack.pop());
    }
    */
}

void GraphMLController::model_WriteToFile(QString filePath, QString data)
{
    qCritical() << "GraphMLController::model_WriteToFile";
    try{
        QFile file(filePath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << data;
        file.close();
        qDebug() << "Successfully written file: " << filePath;
    }catch(...){
        qCritical() << "Export Failed!" ;
    }
}

void GraphMLController::model_EnableGUI(bool enabled)
{

    emit view_EnableGUI(enabled);
}



void GraphMLController::model_MadeEdge(Edge *edge)
{
    qCritical() << "GraphMLController::model_MadeEdge()";
    //Get the source and destination from the edge.
    Node* srcNode = (Node*) edge->getSource();
    Node* dstNode = (Node*) edge->getDestination();

    NodeItem* srcGUI = getNodeItemFromNode(srcNode);
    NodeItem* dstGUI = getNodeItemFromNode(dstNode);


    if(srcGUI != 0 && dstGUI != 0){
        //We have valid GUI elements for both ends of this edge.
        //Make an action for this Operation.
        Action action;
        action.actionType = CONSTRUCT;
        action.itemKind = GraphML::EDGE;
        action.itemID = edge->getID();

        //Add action to the Undo/Redo buffer.
        addActionToStack(action);

        undoLookup[action.itemID] = edge;

        //Construct a new GUI Element for this edge.
        NodeEdge* nodeEdge = new NodeEdge(edge, srcGUI, dstGUI);


        //Connect the controller to the Edge's Signals.
        connect(nodeEdge, SIGNAL(setSelected(NodeEdge*)), this, SLOT(nodeEdge_Selected(NodeEdge*)));

        //Add it to the list of EdgeItems in the Model.
        edgeItems.append(nodeEdge);

        emit view_addNodeEdge(nodeEdge);
    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }
}

void GraphMLController::model_MadeNode(GraphMLContainer *item)
{

    Node* node = dynamic_cast<Node*>(item);

    if(item == 0){
        return;
    }
    QStringList kindsToMake;
    kindsToMake << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort" << "Attribute" << "HardwareNode";


    QString nodeKind = item->getDataValue("kind");


    //If we are meant to make this node.
    if(kindsToMake.contains(nodeKind)){
        Node* node = (Node*) item;
        //Get Visual Parent Node
        NodeItem* parentNodeItem = 0;

        Node* parentNode = node->getParentNode();

        //Get Parent item from lookup hash
        parentNodeItem = getNodeItemFromNode(parentNode);

        //Get ID.
        QString itemID = node->getID();
        QString parentItemID = "";
        if(parentNode != 0){
            parentItemID = parentNode->getID();

        }

        //Insert an action to reverse it.

        Action newNode;
        newNode.actionType = CONSTRUCT;
        newNode.itemKind = item->getKind();
        newNode.itemID = node->getID();
        newNode.srcID = "";
        newNode.dstID = "";
        newNode.removedXML = "";

        addActionToStack(newNode);

        QString previousID = node->getDataValue("previousID");

        if(previousID != ""){
            undoLookup[previousID] = node;
        }
        undoLookup[newNode.itemID] = node;
        /*
        if(!undoIDStack.isEmpty()){
            QString oldID = undoIDStack.pop();
            qCritical() << "Linking old ID: " << oldID << " to: " << node->toString();
            undoLookup[oldID] = node;
        }
        */

        //QString currentID = node->getID();
        //undoLookup[currentID] = node;

        qCritical() << "GG1";

        NodeItem* newNodeItem = new NodeItem(node, parentNodeItem);
        qCritical() << "GG2";

        nodeItems.append(newNodeItem);
        qCritical() << "GG3";

        connect(newNodeItem, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(nodeItem_Selected(NodeItem*)));
        connect(newNodeItem, SIGNAL(centreNode(NodeItem*)),this, SLOT(nodeItem_SetCentered(NodeItem*)));
        connect(newNodeItem, SIGNAL(makeChildNode(QString, Node*)), this, SLOT(nodeItem_MakeChildNode(QString, Node*)));

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
        qCritical() << "GG4";

        GUIContainer* parent = getGUIContainer(parentNode);

        if(parent == 0){
            treeModel->appendRow(newNodeItemData);
        }else{
            parent->modelItem->appendRow(newNodeItemData);
        }

         qCritical() << "GG5";
        //Add Item to view
        emit view_addNodeItem(newNodeItem);
    }else{
        qCritical() << "GraphMLController::model_MadeNodeNew() << Node Kind: " << nodeKind << " not Implemented";
    }
}

void GraphMLController::model_RemoveNode(GraphMLContainer *item, QString ID)
{
    qCritical() << "GraphMLController::model_RemoveNode(): " << ID;

    //GraphML* gml = undoLookup[ID];
    Node* node = (Node*)item;

    GUIContainer* guiContainer = getGUIContainer(node);
    int position = nodeContainers.indexOf(guiContainer);

    //Remove the Node.
    NodeItem* nodeItem = guiContainer->nodeItem;

    qCritical() << "Container: " << position;

    if(guiContainer == 0){
        qCritical() << "No GUI element for this Node.";
    }

    Action newNode;
    newNode.dstID = "";
    newNode.actionType = DESTRUCT;
    newNode.itemKind = GraphML::NODE;
    newNode.itemID = "";
    newNode.srcID = "";

    if(node == 0){
        qCritical() << "GG";
    }

    newNode.parentID = guiContainer->parentNodeID;
    newNode.removedXML = guiContainer->deleteXML;

    addActionToStack(newNode);


    qCritical() << "Adding ID: " << ID << " To Undo stack!";
    //undoIDStack.append(ID);

    //Remove From Tree Model.
    QModelIndex index = guiContainer->modelItem->index();
    treeModel->removeRow(index.row(), index.parent());
    qCritical() << "Removed from the Tree!";


    removeNodeItem(nodeItem);

    qCritical() << "Removed the GUI!";

    //Remove the GUIContainer
    nodeContainers.remove(position);

    qCritical() << "Exiting!!";
}

void GraphMLController::model_RemoveEdge(Edge *edge, QString srcID, QString dstID)
{
    qCritical() << "GraphMLController::model_RemoveEdge()";

    //Make an action for this Operation.
    Action action;
    action.actionType = DESTRUCT;
    action.itemKind = GraphML::EDGE;
    action.itemID = edge->getID();
    action.srcID = srcID;
    action.dstID = dstID;

    //Add action to the Undo/Redo buffer.
    addActionToStack(action);

    //Get the GUI element for this edge.
    NodeEdge* nodeEdge = getNodeEdgeFromEdge(edge);

    undoLookup[action.itemID] = 0;

    //Remove it
    removeNodeEdge(nodeEdge);
}

void GraphMLController::nodeItem_Selected(NodeItem *nodeItem)
{
    UNDOING = false;
    qCritical() << "GraphMLController::NodeItem_Selected";
    if(KEY_CONTROL_DOWN){
        //If we have already got this item in the selected Item list, deselect it.
        if(selectedNodeItems.contains(nodeItem)){
            deselectNodeItems(nodeItem);
            return;
        }
    }else{
        deselectEdgeItems();
    }

    if(KEY_SHIFT_DOWN){
        if(selectedNodeItems.size() == 1){
            //Get the previous Node
            Node* previousNode = selectedNodeItems[0]->node;
            Node* currentNode = nodeItem->node;
            emit model_ConstructEdge(previousNode, currentNode);
            deselectNodeItems();
        }else{

        }

    }

    if(KEY_SHIFT_DOWN == false && KEY_CONTROL_DOWN == false){
        deselectNodeItems();
    }


    selectNodeItem(nodeItem);
}

void GraphMLController::nodeItem_SetCentered(NodeItem *nodeItem)
{
    currentMaximized = nodeItem;
    emit view_centerNodeItem(nodeItem);
}

//Make a child node inside the node.
void GraphMLController::nodeItem_MakeChildNode(QString type, Node *node)
{
    UNDOING = false;
    //Node* childNode = model->
    emit model_ConstructNode(type, node);
}

void GraphMLController::view_updateGraphMLData(Node *node, QString key, QString value)
{
    if(node->getData(key) != 0){
        Action updatedAction;
        updatedAction.actionType = CHANGED;
        updatedAction.itemID = node->getID();
        updatedAction.key = key;
        updatedAction.previousValue = node->getDataValue(key);

        addActionToStack(updatedAction);
        node->updateDataValue(key, value);
    }
}


void GraphMLController::nodeEdge_Selected(NodeEdge *nodeEdge)
{
    UNDOING = false;
    if(KEY_CONTROL_DOWN){
        //If we have already got this Edge in the selected Item list, deselect it.
        if(selectedEdgeItems.contains(nodeEdge)){
            deselectEdgeItems(nodeEdge);
            qCritical() << "LEL";
            return;
        }
    }

    if(KEY_CONTROL_DOWN == false){
        deselectEdgeItems();
        deselectNodeItems();
    }
    selectEdgeItem(nodeEdge);

}

void GraphMLController::view_SetCentered(QModelIndex index)
{
    GUIContainer* item = getGUIContainer(index);
    nodeItem_SetCentered(item->nodeItem);
}

void GraphMLController::view_SetSelected(QModelIndex index)
{
    GUIContainer* item = getGUIContainer(index);
    nodeItem_Selected(item->nodeItem);
}



void GraphMLController::view_ControlPressed(bool isDown)
{

    qCritical() << "GraphMLController::view_ControlPressed::"<<isDown;
    KEY_CONTROL_DOWN = isDown;
}

void GraphMLController::view_ShiftTriggered(bool isDown)
{
    qCritical() << "GraphMLController::view_ShiftTriggered::"<<isDown;

    if(isDown){
        if(selectedNodeItems.size() == 1){
            hideAllMatches(selectedNodeItems[0]->node);
        }else{
            resetMatches();
        }
    }


    KEY_SHIFT_DOWN = isDown;
}

void GraphMLController::view_DeleteTriggered(bool isDown)
{
    if(isDown){
        if(selectedEdgeItems.size() > 0 || selectedNodeItems.size() > 0){
            deleteSelectedEdgeItems();
            deleteSelectedNodeItems();
        }
    }

}

void GraphMLController::view_SelectAll()
{
    qCritical() << "GraphMLController::view_SelectAll()";
    QVector<GraphMLContainer*> children;

    if(currentMaximized == 0){
        children = model->getChildren(0);
    }else{
        children = currentMaximized->node->getChildren(1);
    }

    for(int i = 0; i < children.size(); i++){
        if(children.at(i) != 0){
            Node* node = (Node*)children.at(i);
            NodeItem* nodeItem = getNodeItemFromNode(node);
            if(nodeItem !=0){
                selectNodeItem(nodeItem);
            }
        }
    }
    qCritical() << "!GraphMLController::view_SelectAll()";
}

void GraphMLController::view_EmptyScenePressed()
{
    deselectNodeItems();
    deselectEdgeItems();
    resetMatches();
    currentMaximized = 0;
    emit view_LabelChanged("");
}

void GraphMLController::view_ActionTriggered(QString action)
{
    actionCount++;
    currentAction = action;
    currentActionID = actionCount;
    qCritical() <<"Action [" << actionCount << "]: "<< action;
}


void GraphMLController::deselectNodeItems(NodeItem *selectedNodeItem)
{
    if(selectedNodeItem == 0){
        for(int i = 0; i < selectedNodeItems.size(); i++){
            selectedNodeItems[i]->setDeselected();
        }
        selectedNodeItems.clear();
    }else{
        int position = selectedNodeItems.indexOf(selectedNodeItem);
        selectedNodeItems.removeAt(position);
        selectedNodeItem->setDeselected();
    }
}

void GraphMLController::deselectEdgeItems(NodeEdge *selectedEdgeItem)
{
    if(selectedEdgeItem == 0){
        for(int i = 0; i < selectedEdgeItems.size(); i++){
            selectedEdgeItems[i]->setDeselected();
        }
        selectedEdgeItems.clear();
    }else{
        int position = selectedEdgeItems.indexOf(selectedEdgeItem);
        selectedEdgeItems.removeAt(position);
        selectedEdgeItem->setDeselected();
    }
}

void GraphMLController::selectNodeItem(NodeItem *selectedNodeItem)
{
    if(!selectedNodeItems.contains(selectedNodeItem)){
        selectedNodeItems.append(selectedNodeItem);
    }
    GUIContainer* gui = getGUIContainer(selectedNodeItem);
    selectedNodeItem->setSelected();

    emit view_LabelChanged(gui->node->getDataValue("label"));
}

void GraphMLController::selectEdgeItem(NodeEdge *selectedEdgeItem)
{
    if(!selectedEdgeItems.contains(selectedEdgeItem)){
        selectedEdgeItems.append(selectedEdgeItem);
    }
    selectedEdgeItem->setSelected();
}

bool nodeItemLessThan(NodeItem* o1, NodeItem* o2){
    return o1->depth >= o2->depth;
}

void GraphMLController::deleteSelectedNodeItems()
{
    //Sort so we delete the highest depth first.
    qSort(selectedNodeItems.begin(), selectedNodeItems.end() ,nodeItemLessThan);

    while(selectedNodeItems.size() != 0){
        NodeItem* tdNodeItem = selectedNodeItems.first();
        if(tdNodeItem != 0){
            Node* tdNode = tdNodeItem->node;
            removeGraphML(tdNode);

            selectedNodeItems.pop_front();
        }
    }
    emit view_LabelChanged("");
}

void GraphMLController::deleteSelectedEdgeItems()
{
    while(selectedEdgeItems.size() != 0){
        NodeEdge* tdNodeItem = selectedEdgeItems.first();
        if(tdNodeItem != 0){
            Edge* tdNode = tdNodeItem->edge;
            delete tdNode;
            //delete tdNode;
            selectedEdgeItems.pop_front();
        }
    }

}

GUIContainer *GraphMLController::getGUIContainer(Node *node)
{
    for(int i = 0; i < nodeContainers.size(); i++){
        if(nodeContainers[i]->node == node){
            return nodeContainers[i];
        }
    }
    return 0;
}

GUIContainer *GraphMLController::getGUIContainer(NodeItem *nodeItem)
{
    for(int i = 0; i < nodeContainers.size(); i++){
        if(nodeContainers[i]->nodeItem == nodeItem){
            return nodeContainers[i];
        }
    }
    return 0;
}

GUIContainer *GraphMLController::getGUIContainer(QModelIndex nodeIndex)
{
    QStandardItem* item = treeModel->itemFromIndex(nodeIndex);

    for(int i = 0; i < nodeContainers.size(); i++){
        if(nodeContainers[i]->modelItem == item){
            return nodeContainers[i];
        }
    }
    return 0;
}

void GraphMLController::addActionToStack(Action action)
{
    //Get Current Action ID and action.
    action.actionID = currentActionID;
    action.actionName = currentAction;

    if(!UNDOING){
        undoStack.append(action);
    }else{
        redoStack.append(action);
    }
}




NodeItem *GraphMLController::getNodeItemFromNode(Node *node)
{
    for(int i = 0; i < nodeItems.size(); i++){
        NodeItem* nodeItem = nodeItems[i];
        if(nodeItem != 0 && nodeItem->node == node){
            return nodeItem;
        }
    }
    return 0;
}

NodeEdge *GraphMLController::getNodeEdgeFromEdge(Edge *edge)
{
    for(int i = 0; i < edgeItems.size(); i++){
        NodeEdge* nodeEdge = edgeItems[i];
        if(nodeEdge != 0 && nodeEdge->edge == edge){
            return nodeEdge;
        }
    }
    return 0;

}


void GraphMLController::removeNodeItem(NodeItem *nodeItem)
{
    if(nodeItem != 0){

        int position = nodeItems.indexOf(nodeItem);
        nodeItems.remove(position);

        if(currentMaximized == nodeItem){
            currentMaximized = 0;
        }
        delete nodeItem;
    }
}

void GraphMLController::removeNodeEdge(NodeEdge *nodeEdge)
{
    //If the NodeEdge is not null, remove it from the list.
    if(nodeEdge != 0){
        int position = edgeItems.indexOf(nodeEdge);
        edgeItems.remove(position);
        delete nodeEdge;
    }

}

void GraphMLController::hideAllMatches(Node *node)
{
    QVector<GraphMLContainer *> items = model->getChildren();
    for(int i = 0; i <items.size();i++){
        GraphMLContainer* currentItem = items[i];
        Node* currentNode = (Node*)currentItem;

        GUIContainer* currentContainer = getGUIContainer(currentNode);
        if(currentContainer != 0){
            if(node->isEdgeLegal(currentItem)){
                currentContainer->nodeItem->setOpacity(1);
            }else{
                if(node!= currentContainer->node){
                    currentContainer->nodeItem->setOpacity(.05);
                }
            }
        }
    }

}

void GraphMLController::resetMatches()
{
    QVector<GraphMLContainer *> items = model->getChildren();
    for(int i = 0; i <items.size();i++){
        GraphMLContainer* currentItem = items[i];
        Node* currentNode = (Node*)currentItem;

        GUIContainer* currentContainer = getGUIContainer(currentNode);
        if(currentContainer != 0){
                currentContainer->nodeItem->setOpacity(1);
        }
    }

}
