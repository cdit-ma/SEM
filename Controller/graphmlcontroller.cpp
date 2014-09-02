#include "graphmlcontroller.h"
#include "../GUI/nodeconnection.h"
#include <QDebug>
//Constructor
GraphMLController::GraphMLController(NodeView* view):QObject()
{
    this->model = new Model();
    this->view = view;


    //Setup variables;
    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;

    //Connect View and Model

    currentMaximized=0;


    treeModel = new QStandardItemModel(this);


    //Connect the Signals to the VIEW
    connect(this, SIGNAL(view_addNodeEdge(NodeEdge*)),view,SLOT(addEdgeItem(NodeEdge*)));
    connect(this, SIGNAL(view_addNodeItem(NodeItem*)),view,SLOT(addNodeItem(NodeItem*)));
    connect(this, SIGNAL(view_centerNodeItem(NodeItem*)),view,SLOT(centreItem(NodeItem*)));


    //Connect to the Signals from the VIEW
    connect(view, SIGNAL(controlPressed(bool)), this, SLOT(view_ControlPressed(bool)));
    connect(view, SIGNAL(shiftPressed(bool)), this, SLOT(view_ShiftTriggered(bool)));
    connect(view, SIGNAL(deletePressed(bool)), this, SLOT(view_DeleteTriggered(bool)));
    connect(view, SIGNAL(unselect()),this, SLOT(view_EmptyScenePressed()));
    connect(view, SIGNAL(selectAll()),this, SLOT(view_SelectAll()));
    connect(view, SIGNAL(copy()), this, SLOT(view_Copy()));
    connect(view, SIGNAL(cut()), this, SLOT(view_Cut()));

    //Connect the Signals to the MODEL
    connect(this, SIGNAL(model_ExportGraphML(QString)), model, SLOT(init_ExportGraphML(QString)));
    connect(this, SIGNAL(model_ImportGraphML(QStringList, GraphMLContainer*)), model, SLOT(init_ImportGraphML(QStringList, GraphMLContainer*)));
    connect(this, SIGNAL(model_MakeChildNode(Node*)),model,SLOT(model_MakeChildNode(Node*)));
    connect(model, SIGNAL(view_EnableGUI(bool)), this, SLOT(model_EnableGUI(bool)));

    connect(model, SIGNAL(view_DestructGUINode(GraphMLContainer*)), this, SLOT(model_RemoveNode(GraphMLContainer*)));

    //connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)),this, SLOT(deleteComponent(GraphMLContainer*)));

    //connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)), this, SLOT(model_RemoveNode(GraphMLContainer*)));

    //Connect to the Signals of the MODEL
    connect(model, SIGNAL(view_ConstructGUINode(GraphMLContainer*)), this,SLOT(model_MadeNodeNew(GraphMLContainer*)));
    //connect(model,SIGNAL(constructNodeItem(Node*)),this,SLOT(model_MakeNode(Node*)));
    connect(model,SIGNAL(view_ConstructGUIEdge(Edge*)),this,SLOT(model_MakeEdge(Edge*)));




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
    qCritical() << "WHAT IS GOING ON";
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
        selectedNodeItems[0]->node->updateDataValue("label", value);
        GUIContainer* gi = getGUIContainer(selectedNodeItems[0]);
        gi->modelItem->setText(value);
    }
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

void GraphMLController::model_MakeNode(Node *node)
{
    qCritical() << "GraphMLController: Making Node!";

    QStringList kindsToMake;
    kindsToMake << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort" << "Attribute";

    qCritical() << "GraphMLController: kindsToMake Node!";
    QString nodeKind = node->getDataValue("kind");

    //If we are meant to make this node.
    if(kindsToMake.contains(nodeKind)){
         qCritical() << "GraphMLController: kindsToMake Node!";

        //Get Visual Parent Node
        NodeItem* parentNodeItem = 0;

        Node* parentNode = node->getParentNode();

        //Get Parent item from lookup hash
        parentNodeItem = getNodeItemFromNode(parentNode);

        NodeItem* newNodeItem = new NodeItem(node, parentNodeItem);

        qCritical() << "APPENDAGE!";
        nodeItems.append(newNodeItem);


        connect(newNodeItem, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(nodeItem_Selected(NodeItem*)));

        connect(newNodeItem, SIGNAL(centreNode(NodeItem*)),this, SLOT(nodeItem_SetCentered(NodeItem*)));

        connect(newNodeItem, SIGNAL(makeChildNode(Node*)), this, SLOT(nodeItem_MakeChildNode(Node*)));

        //Add item to the Tree Model.
        QStandardItem* newNodeItemData = new QStandardItem(node->getDataValue("label"));
        newNodeItemData->setFlags(newNodeItemData->flags() & ~Qt::ItemIsEditable);

        GUIContainer* toAppend = new GUIContainer();
        toAppend->node = node;
        toAppend->nodeItem=newNodeItem;
        toAppend->modelItem=newNodeItemData;
        nodeContainers.append(toAppend);

        GUIContainer* parent = getGUIContainer(parentNode);
        if(parent == 0){
            treeModel->appendRow(newNodeItemData);
        }else{
            parent->modelItem->appendRow(newNodeItemData);
        }

        //Add Item to view
        emit view_addNodeItem(newNodeItem);
    }else{
        qCritical() << "GraphMLController: Not Making Node!";
        //Don't make
    }
}

void GraphMLController::model_MakeEdge(Edge *edge)
{
    GraphMLContainer* source = edge->getSource();
    GraphMLContainer* destination = edge->getDestination();

    NodeItem* sourceItem = getNodeItemFromNode((Node*)source);
    NodeItem* destinationItem = getNodeItemFromNode((Node*)destination);



    if(sourceItem != 0 && destinationItem != 0){
        NodeEdge* nC = new NodeEdge(edge, sourceItem, destinationItem);
        nC->addToScene(view->scene());

        connect(nC, SIGNAL(setSelected(NodeEdge*)), this, SLOT(nodeEdge_Selected(NodeEdge*)));
    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }
}

void GraphMLController::model_MadeNodeNew(GraphMLContainer *item)
{
    qDebug() << "GraphMLController::model_MadeNodeNew()";

    if(item == 0){
        return;
    }
    QStringList kindsToMake;
    kindsToMake << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort" << "Attribute";


    QString nodeKind = item->getDataValue("kind");


    //If we are meant to make this node.
    if(kindsToMake.contains(nodeKind)){
        Node* node = (Node*) item;
        //Get Visual Parent Node
        NodeItem* parentNodeItem = 0;

        Node* parentNode = node->getParentNode();

        //Get Parent item from lookup hash
        parentNodeItem = getNodeItemFromNode(parentNode);

        NodeItem* newNodeItem = new NodeItem(node, parentNodeItem);

        nodeItems.append(newNodeItem);

        connect(newNodeItem, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(nodeItem_Selected(NodeItem*)));

        connect(newNodeItem, SIGNAL(centreNode(NodeItem*)),this, SLOT(nodeItem_SetCentered(NodeItem*)));

        connect(newNodeItem, SIGNAL(makeChildNode(Node*)), this, SLOT(nodeItem_MakeChildNode(Node*)));

        //Add item to the Tree Model.
        QStandardItem* newNodeItemData = new QStandardItem(node->getDataValue("label"));
        newNodeItemData->setFlags(newNodeItemData->flags() & ~Qt::ItemIsEditable);

        GUIContainer* toAppend = new GUIContainer();
        toAppend->node = node;
        toAppend->nodeItem=newNodeItem;
        toAppend->modelItem=newNodeItemData;
        nodeContainers.append(toAppend);

        GUIContainer* parent = getGUIContainer(parentNode);

        if(parent == 0){
            treeModel->appendRow(newNodeItemData);
        }else{
            parent->modelItem->appendRow(newNodeItemData);
        }

        //Add Item to view
        emit view_addNodeItem(newNodeItem);
    }else{
        qCritical() << "GraphMLController::model_MadeNodeNew() << Node Kind: " << nodeKind << " not Implemented";
    }
}

void GraphMLController::model_RemoveNode(GraphMLContainer *item)
{
    qCritical() << "GraphMLController::model_RemoveNode()";
    //Removes the item from the list of built nodes.
    Node* node = (Node*)item;
    if(node != 0){
        GUIContainer* item = getGUIContainer(node);
        if(item != 0){
            int position = nodeContainers.indexOf(item);

            QModelIndex index = item->modelItem->index();
            treeModel->removeRow(index.row(), index.parent());


            removeNodeItem(item->nodeItem);


            nodeContainers.removeAt(position);
        }
    }
}

void GraphMLController::nodeItem_Selected(NodeItem *nodeItem)
{
    qCritical() << "GraphMLController::NodeItem_Selected";
    if(KEY_CONTROL_DOWN){
        //If we have already got this item in the selected Item list, deselect it.
        if(selectedNodeItems.contains(nodeItem)){
            deselectNodeItems(nodeItem);
            return;
        }
    }else{
    }

    if(KEY_SHIFT_DOWN){
        if(selectedNodeItems.size() == 1){
            Node* node1 = selectedNodeItems[0]->node;
            if(node1->isEdgeLegal(nodeItem->node)){
                Edge* edge = new Edge(node1, nodeItem->node);
                model_MakeEdge(edge);
                this->resetMatches();
                return;
            }else{
                deselectNodeItems();
            }
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
void GraphMLController::nodeItem_MakeChildNode(Node *node)
{
    emit model_MakeChildNode(node);
}


void GraphMLController::nodeEdge_Selected(NodeEdge *nodeEdge)
{
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
        deleteSelectedEdgeItems();
        deleteSelectedNodeItems();
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
    qSort(selectedNodeItems.begin(), selectedNodeItems.end(),nodeItemLessThan);

    while(selectedNodeItems.size() != 0){
        NodeItem* tdNodeItem = selectedNodeItems.first();
        if(tdNodeItem != 0){
            Node* tdNode = tdNodeItem->node;
            delete tdNode;
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


void GraphMLController::removeNodeItem(NodeItem *nodeItem)
{
    if(nodeItem != 0){
        int position = nodeItems.indexOf(nodeItem);
        nodeItems.remove(position);

        if(currentMaximized == nodeItem){
            currentMaximized = 0;
        }

    }
    delete nodeItem;
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
