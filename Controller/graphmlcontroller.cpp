#include "graphmlcontroller.h"
#include "../GUI/nodeconnection.h"
#include <QDebug>
//Constructor
GraphMLController::GraphMLController(Model *model, NodeView* view):QObject()
{

    //Setup variables;
    KEY_CONTROL_DOWN = false;
    KEY_SHIFT_DOWN = false;

    //Connect View and Model
    this->model = model;
    this->view = view;

    currentNodeAspect=0;


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

    //Connect the Signals to the MODEL
    connect(this, SIGNAL(model_ExportGraphML(QString)), model, SLOT(init_ExportGraphML(QString)));
    connect(this, SIGNAL(model_ImportGraphML(QStringList)), model, SLOT(init_ImportGraphML(QStringList)));
    connect(this, SIGNAL(model_MakeChildNode(Node*)),model,SLOT(model_MakeChildNode(Node*)));

    //connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)),this, SLOT(deleteComponent(GraphMLContainer*)));

    connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)), this, SLOT(model_RemoveNode(GraphMLContainer*)));

    //Connect to the Signals of the MODEL
    connect(model, SIGNAL(constructNodeItemNew(GraphMLContainer*)), this,SLOT(model_MadeNodeNew(GraphMLContainer*)));
    //connect(model,SIGNAL(constructNodeItem(Node*)),this,SLOT(model_MakeNode(Node*)));
    connect(model,SIGNAL(constructEdgeItem(Edge*)),this,SLOT(model_MakeEdge(Edge*)));




    connect(model,SIGNAL(returnExportedGraphMLData(QString,QString)), this, SLOT(model_WriteToFile(QString,QString)));
}

GraphMLController::~GraphMLController()
{

}

QStandardItemModel *GraphMLController::getTreeModel()
{
    return this->treeModel;
}

void GraphMLController::view_ImportGraphML(QStringList inputGraphML)
{
    qCritical() << "GraphMLController::view_ImportGraphML";
    emit model_ImportGraphML(inputGraphML);
}

void GraphMLController::view_ExportGraphML(QString filePath)
{
    qCritical() << "GraphMLController::view_ExportGraphML";
    emit model_ExportGraphML(filePath);
}

void GraphMLController::view_UpdateLabel(QString value)
{
    if(selectedNodeItems.size() == 1){
        selectedNodeItems[0]->node->setDataValue("label", value);
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

void GraphMLController::model_MakeNode(Node *node)
{
    qCritical() << "GraphMLController: Making Node!";

    QStringList kindsToMake;
    kindsToMake << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort";

    QString nodeKind = node->getDataValue("kind");

    //If we are meant to make this node.
    if(kindsToMake.contains(nodeKind)){

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
        qCritical() << "GraphMLController: Not Making Node!";
        //Don't make
    }
}

void GraphMLController::model_MakeEdge(Edge *edge)
{
    qCritical() << "GraphMLController: Making Edge!";

    GraphMLContainer* source = edge->getSource();
    GraphMLContainer* destination = edge->getDestination();


    NodeItem* sourceItem = getNodeItemFromNode((Node*)source);
    NodeItem* destinationItem = getNodeItemFromNode((Node*)destination);

    if(sourceItem != 0 && destinationItem != 0){
        NodeEdge* nC = new NodeEdge(edge, sourceItem, destinationItem);
        nC->addToScene(view->scene());
    }
}

void GraphMLController::model_MadeNodeNew(GraphMLContainer *item)
{
    qCritical() << "GraphMLController: Making Node!";

    QStringList kindsToMake;
    kindsToMake << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort";

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
            qCritical() << "YOLO";
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

void GraphMLController::model_RemoveNode(GraphMLContainer *item)
{
    //Removes the item from the list of built nodes.
    qCritical() << "GraphMLController::model_RemoveNode";
    Node* node = (Node*)item;
    if(node != 0){
        GUIContainer* item = getGUIContainer(node);
        if(item != 0){
            int position = nodeContainers.indexOf(item);

            QModelIndex index = item->modelItem->index();
            treeModel->removeRow(index.row(), index.parent());

            nodeContainers.removeAt(position);
        }
        NodeItem* nodeItem = getNodeItemFromNode(node);
        removeNodeItem(nodeItem);
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
    currentNodeAspect = nodeItem;
    emit view_centerNodeItem(nodeItem);
}

//Make a child node inside the node.
void GraphMLController::nodeItem_MakeChildNode(Node *node)
{
    emit model_MakeChildNode(node);
}


void GraphMLController::nodeEdge_Selected(NodeEdge *nodeEdge)
{

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
    }}else{
        resetMatches();
    }


    KEY_SHIFT_DOWN = isDown;
}

void GraphMLController::view_DeleteTriggered(bool isDown)
{
    if(isDown){
        deleteSelectedNodeItems();
    }

}

void GraphMLController::view_SelectAll()
{
    qCritical() << "GraphMLController::view_SelectAll()";
    QVector<GraphMLContainer*> children;

    if(currentNodeAspect ==0){
        children = model->getGraph()->getChildren(0);
    }else{
        children = currentNodeAspect->node->getChildren(1);
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
    resetMatches();
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

void GraphMLController::selectNodeItem(NodeItem *selectedNodeItem)
{
    if(!selectedNodeItems.contains(selectedNodeItem)){
        selectedNodeItems.append(selectedNodeItem);
    }
    GUIContainer* gui = getGUIContainer(selectedNodeItem);
    selectedNodeItem->setSelected();

    emit view_LabelChanged(gui->node->getDataValue("label"));
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
        if(nodeItem->node == node){
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

        if(currentNodeAspect == nodeItem){
            currentNodeAspect = 0;
        }
    }
}

void GraphMLController::hideAllMatches(Node *node)
{
    QVector<GraphMLContainer *> items = this->model->getGraph()->getChildren(-1);
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
    QVector<GraphMLContainer *> items = this->model->getGraph()->getChildren(-1);
    for(int i = 0; i <items.size();i++){
        GraphMLContainer* currentItem = items[i];
        Node* currentNode = (Node*)currentItem;

        GUIContainer* currentContainer = getGUIContainer(currentNode);
        if(currentContainer != 0){
                currentContainer->nodeItem->setOpacity(1);
        }
    }

}
