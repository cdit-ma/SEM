#include "graphmlcontroller.h"
#include "../GUI/nodeconnection.h"
#include <QDebug>

//Constructor
GraphMLController::GraphMLController(NodeView* view):QObject()
{
    model = new Model();
    treeModel = new QStandardItemModel(this);

    componentTypes << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort" << "Attribute" << "HardwareNode" << "HardwareCluster" << "PeriodicEvent" << "Component" << "Member";

    aspects << "Assembly" << "Workload";

    this->view = view;

    //Setup variables;
    UNDOING = false;
    REDOING = false;

    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;

    //Connect View and Model
    currentMaximized=0;
    currentActionID = 0;
    actionCount = 0;

    currentAction = "";



    //Connect the Signals to the VIEW
    connect(this, SIGNAL(view_addNodeEdge(NodeEdge*)), view, SLOT(addEdgeItem(NodeEdge*)));
    connect(this, SIGNAL(view_addNodeItem(NodeItem*)), view, SLOT(addNodeItem(NodeItem*)));
    connect(this, SIGNAL(view_centerNodeItem(NodeItem*)), view, SLOT(centreItem(NodeItem*)));

    connect(this, SIGNAL(view_SetAspect(QString)), view, SLOT(setViewAspect(QString)));

    //Connect to the Signals from the VIEW
    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftTriggered(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeleteTriggered(bool)));

    connect(view, SIGNAL(constructNodeItem(QPointF)), this, SLOT(nodeItem_MakeChildNode(QPointF)));

    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(unselect()),this, SLOT(view_EmptyScenePressed()));

    connect(view, SIGNAL(cut()), this, SLOT(view_Cut()));
    connect(view, SIGNAL(copy()), this, SLOT(view_Copy()));

    connect(view, SIGNAL(undo()), this, SLOT(view_Undo()));
    connect(view, SIGNAL(redo()), this, SLOT(view_Redo()));

    //Connect the Signals to the Model.
    connect(this, SIGNAL(model_ConstructNode(QPointF, QString, GraphMLContainer*)), model, SLOT(view_ConstructNode(QPointF, QString, GraphMLContainer*)));
    connect(this, SIGNAL(model_ConstructEdge(GraphMLContainer*, GraphMLContainer*)), model, SLOT(view_ConstructEdge(GraphMLContainer*, GraphMLContainer*)));
    connect(this, SIGNAL(model_ImportGraphML(QStringList, GraphMLContainer*)), model, SLOT(view_ImportGraphML(QStringList, GraphMLContainer*)));
    connect(this, SIGNAL(model_ImportGraphML(QString, GraphMLContainer*)), model, SLOT(view_ImportGraphML(QString, GraphMLContainer*)));
    connect(this, SIGNAL(model_ExportGraphML(QString)), model, SLOT(view_ExportGraphML(QString)));

    connect(this, SIGNAL(view_addNodeItem(NodeItem*)), model, SLOT(view_Constructed()));
    connect(this, SIGNAL(view_addNodeEdge(NodeEdge*)), model, SLOT(view_Constructed()));

    connect(this, SIGNAL(model_ConstructNodeInstance(GraphMLContainer*)), model, SLOT(view_ConstructNodeInstance(GraphMLContainer*)));
    //Connect to the Signals from the Model.
    connect(model, SIGNAL(view_EnableGUI(bool)), this, SLOT(model_EnableGUI(bool)));
    connect(model, SIGNAL(controller_ActionTrigger(QString)), this, SLOT(view_ActionTriggered(QString)));

    connect(model, SIGNAL(view_ConstructGUINode(GraphMLContainer*)), this,SLOT(model_MadeNode(GraphMLContainer*)));
    connect(model, SIGNAL(view_DestructGUINode(GraphMLContainer*, QString)), this, SLOT(model_RemoveNode(GraphMLContainer*, QString)));

    connect(model, SIGNAL(view_ConstructGUIEdge(Edge*)),this,SLOT(model_MadeEdge(Edge*)));
    connect(model, SIGNAL(view_DestructGUIEdge(Edge*, QString,QString, QString)), this, SLOT(model_RemoveEdge(Edge*, QString, QString, QString)));

    connect(model, SIGNAL(view_ReturnExportedData(QString,QString)), this, SLOT(model_WriteToFile(QString,QString)));

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




void GraphMLController::view_Copy()
{
    QVector<GraphMLContainer*> nodes;

    //Get the Model's Node object for each selected Node Items.
    foreach(NodeItem* nodeItem, selectedNodeItems){
        if(nodeItem != 0){
            if(nodeItem->node!=0){
                nodes.append(nodeItem->node);
            }
        }
    }

    //Export the GraphML for those Nodes.
    QString result = model->exportGraphML(nodes);

    //Tell the view to place the resulting GraphML String into the Copy buffer.
    emit view_CopyData(result);
}

void GraphMLController::view_Cut()
{
    //Run Copy
    view_Copy();

    //Then remove the selected node items.
    deleteSelectedNodeItems();
}

void GraphMLController::view_MakeInstance()
{
    //Get the currentParent
    GraphMLContainer* currentParent =  getSingleSelectedNode();

    if(currentParent != 0){
        emit model_ConstructNodeInstance(currentParent);
    }

}

void GraphMLController::view_Paste(QString XMLData)
{
    //Get the currentParent
    GraphMLContainer* currentParent =  getSingleSelectedNode();

    //Paste into the current Maximized Node.
    if(currentMaximized != 0){
        currentParent = currentMaximized->node;
    }


    emit model_ImportGraphML(XMLData, currentParent);
}


QStringList GraphMLController::getComponentKinds()
{
    return componentTypes;
}

QStringList GraphMLController::getViewAspects()
{
    return aspects;
}

void GraphMLController::view_ImportGraphML(QStringList inputGraphML)
{
    qCritical() << "GraphMLController::view_ImportGraphML";
    emit model_ImportGraphML(inputGraphML);
}

void GraphMLController::view_ImportGraphML(QString inputGraphML)
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
    Node* selectedNode = (Node*)getSingleSelectedNode();

    if(selectedNode != 0){
        view_ActionTriggered("Updating Label");
        view_updateGraphMLData(selectedNode, "label", value);
    }
}


void GraphMLController::reverseAction(Action action)
{
    qCritical() << "GraphMLController::reverseAction()" << action.actionID << " : " << action.actionName;

    switch(action.actionType){

    case CONSTRUCT:{
        //Reverse a Construct action, so Destruct it.
        //Get the constructed Item from the ID specified and then remove it.
        GraphML* item = undoLookup[action.itemID];
        removeGraphML(item);
        break;
    }

    case DESTRUCT:{
        //Reverse a Destruct action, so Construct it.
        switch(action.itemKind){

        case GraphML::NODE:{
            GraphMLContainer* parent = 0;

            if(action.parentID != ""){
                qCritical() << "Looking for parent Component ID: " << action.parentID;
                parent = (GraphMLContainer*) undoLookup[action.parentID];
            }

            if(action.removedXML != ""){
                qCritical() << "Attempting to import GraphML to reverse destruct.";
                model->importGraphML(action.removedXML, parent);
                //emit model_ImportGraphML(action.removedXML, parent);
            }else{
                qCritical() << "Action for item ID: " << action.itemID << " has no stored GraphML data.";
            }
            break;
        }
        case GraphML::EDGE:{
            qCritical() << "Attempting to construct Edge: " << action.srcID << " <-> " << action.dstID;

            //Get the source and destination components.
            GraphMLContainer* src = (GraphMLContainer*) undoLookup[action.srcID];
            GraphMLContainer* dst = (GraphMLContainer*) undoLookup[action.dstID];

            if(src != 0 && dst != 0){
                qCritical() << "Constructing Edge: " << src->getID() << " <-> " << dst->getID();
                emit model_ConstructEdge(src, dst);
            }else{
                qCritical() << "Source or Destination Component does not exist!";
            }
            break;
        }
        default:
            qCritical() << "Action.itemKind: " << action.itemKind << " not implemented.";
            break;
        }
        break;
    }
    case MODIFY:{
        qCritical() << "Attribute Modified";
        //Reverse an attribute change.
        //Get the node which data has changed.
        qCritical() << "Attribute Modified" << action.itemID;
        Node* node = (Node*)undoLookup[action.itemID];
        qCritical() << node->getID();

        //Reverse the change of GraphMLData
        emit view_updateGraphMLData(node, action.key, action.previousValue);
        break;
    }
    default:
        qCritical() << "Action.actionType: " << action.actionType << " no implemented.";
        break;
    }



}

void GraphMLController::removeGraphML(GraphML *graph)
{
    if(graph == 0){
        return;
    }

    GraphMLContainer* node = dynamic_cast<GraphMLContainer*>(graph);

    if(node != 0){
        GUIContainer* item = getGUIContainer((Node*)node);

        if(item != 0){
            //Export the GraphML for the Node.
            item->deleteXML = model->exportGraphML(node);

            //Get the Nodes Parent ID.
            Node* parentNode = item->node->getParentNode();

            if(parentNode != 0){
                item->parentNodeID = parentNode->getID();
            }else{
                item->parentNodeID = "";
            }
        }else{
            //Took Too long?
            qCritical() << "THis is is interesting";
        }
    }
    delete graph;
}


void GraphMLController::view_Undo()
{
    undoRedo(true);
}

void GraphMLController::view_Redo()
{
    undoRedo(false);
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
    //qCritical() << "GraphMLController::model_MadeEdge()";
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

        if(!undoIDStack.isEmpty()){
            QString oldID = undoIDStack.pop();
            qCritical() << "Linking old ID: " << oldID << " to: " << edge->toString();
            undoLookup[oldID] = edge;
        }


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

    if(node == 0){
        return;
    }

    QString nodeKind = item->getDataValue("kind");

    //If we are meant to make this node.
    if(componentTypes.contains(nodeKind) || true){
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
            //qCritical() << parentNode->getID();

        }else{
           // qCritical() << "No Parent yet?!";
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

        undoLookup[newNode.itemID] = node;

        if(!undoIDStack.isEmpty()){
            QString oldID = undoIDStack.pop();
            qCritical() << "Linking old ID: " << oldID << " to: " << node->toString();
            undoLookup[oldID] = node;
        }

        NodeItem* newNodeItem = new NodeItem(node, parentNodeItem);

        nodeItems.append(newNodeItem);

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
    newNode.itemID = ID;
    newNode.srcID = "";

    if(node == 0){
        qCritical() << "Cannot find node.";
    }

    newNode.parentID = guiContainer->parentNodeID;
    newNode.removedXML = guiContainer->deleteXML;

    addActionToStack(newNode);

    //This should be the next item to make during an undo.
    qCritical() << "Adding ID: " << ID << " To Undo stack!";
    undoIDStack.append(ID);

    //Remove the pointer to it.
    undoLookup[ID] = 0;

    //Remove From Tree Model.
    QModelIndex index = guiContainer->modelItem->index();
    treeModel->removeRow(index.row(), index.parent());
    qCritical() << "Removed from the Tree!";


    removeNodeItem(nodeItem);

    qCritical() << "Removed the GUI!";

    //Remove the GUIContainer
    nodeContainers.remove(position);

    qCritical() << nodeContainers.size();
    qCritical() << selectedEdgeItems.size();
    qCritical() << nodeItems.size();
    qCritical() << edgeItems.size();
    qCritical() << treeModel->rowCount();
    qCritical() << (currentMaximized == 0);
    qCritical() << (currentParent == 0);
}

void GraphMLController::model_RemoveEdge(Edge *edge, QString ID, QString srcID, QString dstID)
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

    undoLookup[ID] = 0;

    undoIDStack.push(ID);

    //Remove it
    removeNodeEdge(nodeEdge);
}

void GraphMLController::nodeItem_Selected(NodeItem *nodeItem)
{
    UNDOING = false;
    qCritical() << "GraphMLController::NodeItem_Selected";
    if(KEY_CONTROL_DOWN){
        //If we have already got this item in the selected Item list, deselect it.
        qCritical() << "1";
        if(selectedNodeItems.contains(nodeItem)){
            qCritical() << "2";
            deselectNodeItems(nodeItem);
            qCritical() << "3";
            return;
        }
    }else{
        qCritical() << "4";
        deselectEdgeItems();
        qCritical() << "5";
    }

    if(KEY_SHIFT_DOWN){

         qCritical() << "6";
        if(selectedNodeItems.size() == 1){
             qCritical() << "7";
            //Get the previous Node
            Node* previousNode = selectedNodeItems[0]->node;
            Node* currentNode = nodeItem->node;
            emit model_ConstructEdge(previousNode, currentNode);
            deselectNodeItems();
        }else{

        }

    }

    qCritical() << "8";


    if(KEY_SHIFT_DOWN == false && KEY_CONTROL_DOWN == false){
        qCritical() << "8.5";
        deselectNodeItems();
    }

     qCritical() << "9";

    selectNodeItem(nodeItem);
    qCritical() << "10";
}

void GraphMLController::nodeItem_SetCentered(NodeItem *nodeItem)
{
    currentMaximized = nodeItem;
    emit view_centerNodeItem(nodeItem);
}

void GraphMLController::nodeItem_MakeChildNode(QPointF centerPoint, Node *node)
{
    UNDOING = false;

    if(childNodeType != ""){
        view_ActionTriggered("Adding a child Node!");
        emit model_ConstructNode(centerPoint, childNodeType, node);
    }

}

void GraphMLController::view_SetChildNodeType(QString nodeType)
{
    childNodeType = nodeType;
}

void GraphMLController::view_SetViewAspect(QString aspect)
{
    emit view_SetAspect(aspect);
}



void GraphMLController::view_updateGraphMLData(Node *node, QString key, QString value)
{
    if(node != 0 && node->getData(key) != 0){
        Action updatedAction;
        updatedAction.actionType = MODIFY;
        updatedAction.itemKind = GraphML::DATA;
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


void GraphMLController::undoRedo(bool UNDO)
{
    QStack<Action> reverseStack;

    UNDOING = UNDO;
    if(UNDO){
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
    emit view_ActionTriggered(actionName);

    qCritical() << "Current Action: " << actionID;
    //Reverse all the actions which match the actionID.

    while(reverseStack.size() > 0){
        Action action = reverseStack.pop();

        qCritical() << "Action: " << action.actionID;
        //If this action is part of the currentAction reverse it.
        if(action.actionID == actionID){
            reverseAction(action);
        }else{
            //currentActionID = action.actionID;
            //currentAction = action.actionName;
            reverseStack.append(action);
            break;
        }
    }

    if(UNDO){
        undoStack = reverseStack;
    }else{
        redoStack = reverseStack;
    }
    updateActionCount();
}



void GraphMLController::view_ControlPressed(bool isDown)
{

    KEY_CONTROL_DOWN = isDown;
}

void GraphMLController::view_ShiftTriggered(bool isDown)
{

    if(isDown){
        Node* selectedNode = (Node*) getSingleSelectedNode();
        if(selectedNode){
            hideAllMatches(selectedNode);
        }else{
            resetMatches();
        }
    }else{
        resetMatches();
    }

    KEY_SHIFT_DOWN = isDown;
}

void GraphMLController::view_DeleteTriggered(bool isDown)
{
    if(isDown){
        if(selectedEdgeItems.size() > 0 || selectedNodeItems.size() > 0){
            view_ActionTriggered("Deleting Node Items.");
            deleteSelectedEdgeItems();
            deleteSelectedNodeItems();
        }
    }

}

void GraphMLController::view_SelectAll()
{
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
}

void GraphMLController::view_EmptyScenePressed()
{
    deselectNodeItems();
    deselectEdgeItems();
    resetMatches();
    currentMaximized = 0;
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
        qCritical() << "1";
        for(int i = 0; i < selectedNodeItems.size(); i++){
            qCritical() << "2";
            if(selectedNodeItems[i] != 0){
                qCritical() << "3";
                selectedNodeItems[i]->setDeselected2();
            }
        }
        qCritical() << "4";
        selectedNodeItems.clear();
    }else{
        qCritical() << "5";
        int position = selectedNodeItems.indexOf(selectedNodeItem);
        selectedNodeItems.removeAt(position);
        selectedNodeItem->setDeselected2();
        qCritical() << "6";
    }
}

void GraphMLController::deselectEdgeItems(NodeEdge *selectedEdgeItem)
{
    if(selectedEdgeItem == 0){
        for(int i = 0; i < selectedEdgeItems.size(); i++){
            //selectedEdgeItems[i]->setDeselected();
        }
        selectedEdgeItems.clear();
    }else{
        int position = selectedEdgeItems.indexOf(selectedEdgeItem);
        selectedEdgeItems.removeAt(position);
        //selectedEdgeItem->setDeselected();
    }
}

void GraphMLController::selectNodeItem(NodeItem *selectedNodeItem)
{
    if(selectedNodeItem != 0){
        if(!selectedNodeItems.contains(selectedNodeItem)){
            selectedNodeItems.append(selectedNodeItem);
        }
        GUIContainer* gui = getGUIContainer(selectedNodeItem);
        selectedNodeItem->setSelected(true);

        emit view_SetAttributeModel(selectedNodeItem->getAttributeTable());
    }
}

void GraphMLController::selectEdgeItem(NodeEdge *selectedEdgeItem)
{
    if(!selectedEdgeItems.contains(selectedEdgeItem)){
        selectedEdgeItems.append(selectedEdgeItem);
    }
    //selectedEdgeItem->setSelected();
}

GraphMLContainer *GraphMLController::getSingleSelectedNode()
{
    NodeItem* nodeItem = getSingleSelectedNodeItem();

    if(nodeItem != 0){
        return nodeItem->node;
    }
    return 0;
}

NodeItem *GraphMLController::getSingleSelectedNodeItem()
{
    if(selectedNodeItems.size() == 1){
        return selectedNodeItems[0];
    }
    return 0;
}

bool GraphMLController::isEdgeLegal(Node *src, Node *dst)
{
    if(src && dst){
        return src->isEdgeLegal(dst) && dst->isEdgeLegal(src);
    }
    return false;
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
            qCritical() << "Removing: " << tdNode->getID();
            removeGraphML(tdNode);
            selectedNodeItems.pop_front();
        }
    }
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

    foreach(NodeItem* nI, selectedNodeItems){
        QVector<Edge*> edges = nI->node->getEdges();
        foreach(Edge* edge, edges){
            delete edge;
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

void GraphMLController::updateActionCount()
{
    QVector<int> temp;
    QStringList undoList;

    foreach(Action a, undoStack){
        if(!temp.contains(a.actionID) && a.actionID != 0){
            temp.append(a.actionID);
            undoList <<a.actionName;
        }
    }
    emit view_UndoCommandList(undoList);

    undoList.clear();
    temp.clear();
    foreach(Action a, redoStack){
        if(!temp.contains(a.actionID) && a.actionID != 0){
            temp.append(a.actionID);
            undoList << a.actionName;
        }
    }

    emit view_RedoCommandList(undoList);
}

void GraphMLController::addActionToStack(Action action)
{
    //Get Current Action ID and action.
    action.actionID = currentActionID;
    action.actionName = currentAction;

    if(UNDOING){
        redoStack.push(action);
    }else{
        undoStack.push(action);
    }
    updateActionCount();
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
            if(node->isEdgeLegal(currentItem) && currentItem->isEdgeLegal(node)){
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
