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
    //connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)),this, SLOT(deleteComponent(GraphMLContainer*)));

    connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)), this, SLOT(model_RemoveNode(GraphMLContainer*)));

    //Connect to the Signals of the MODEL
    connect(model,SIGNAL(constructNodeItem(Node*)),this,SLOT(model_MakeNode(Node*)));
    connect(model,SIGNAL(constructEdgeItem(Edge*)),this,SLOT(model_MakeEdge(Edge*)));




    connect(model,SIGNAL(returnExportedGraphMLData(QString,QString)), this, SLOT(model_WriteToFile(QString,QString)));
}

GraphMLController::~GraphMLController()
{

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
        if(parentNode != 0){
            //Get Parent item from lookup hash
            if(nodeItemLookup.contains(parentNode)){
                parentNodeItem = nodeItemLookup.value(parentNode);
            }
        }

        NodeItem* newNodeItem = new NodeItem(node, parentNodeItem);


        connect(newNodeItem, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(nodeItem_Selected(NodeItem*)));

        connect(newNodeItem, SIGNAL(centreNode(NodeItem*)),this, SLOT(nodeItem_SetCentered(NodeItem*)));
        //Add Item to view
        emit view_addNodeItem(newNodeItem);

        nodeItemLookup.insert(node, newNodeItem);
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

    NodeItem* sourceItem = nodeItemLookup[(Node*)source];
    NodeItem* destinationItem = nodeItemLookup[(Node*)destination];

    if(sourceItem != 0 && destinationItem != 0){
        NodeEdge* nC = new NodeEdge(edge, sourceItem, destinationItem);
        nC->addToScene(view->scene());
    }
}

void GraphMLController::view_RemoveNodeItem(NodeItem *node)
{
    qCritical() << "GraphMLController::model_RemoveNode";

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
                return;
            }else{
                 deselectNodeItems();
            }
        }

    }

    if(KEY_SHIFT_DOWN == false && KEY_CONTROL_DOWN == false){
        deselectNodeItems();
    }


    selectNodeItem(nodeItem);
}

void GraphMLController::nodeItem_SetCentered(NodeItem *nodeItem)
{
    emit view_centerNodeItem(nodeItem);
}


void GraphMLController::nodeEdge_Selected(NodeEdge *nodeEdge)
{

}



void GraphMLController::view_ControlPressed(bool isDown)
{

    qCritical() << "GraphMLController::view_ControlPressed::"<<isDown;
    KEY_CONTROL_DOWN = isDown;
}

void GraphMLController::view_ShiftTriggered(bool isDown)
{
    qCritical() << "GraphMLController::view_ShiftTriggered::"<<isDown;
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

    model->getGraph();

    QList<NodeItem*> items = nodeItemLookup.values();
    qCritical() << items.size();
    for(int i = 0; i < items.size(); i++){
        if(items.at(i) != 0){
            selectNodeItem(items[i]);
        }
    }
    qCritical() << "!GraphMLController::view_SelectAll()";
}

void GraphMLController::view_EmptyScenePressed()
{
    deselectNodeItems();
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
    selectedNodeItem->setSelected();
}

bool nodeItemLessThan(NodeItem* o1, NodeItem* o2){
    return o1->depth >= o2->depth;
}

void GraphMLController::deleteSelectedNodeItems()
{
    qSort(selectedNodeItems.begin(), selectedNodeItems.end(),nodeItemLessThan);

    for(int i = 0; i < selectedNodeItems.size(); i++){
        NodeItem* tdNodeItem = selectedNodeItems[i];
        Node* tdNode = tdNodeItem->node;
        delete tdNode;
    }
    qCritical() << "nodeItemLookup: " << nodeItemLookup.size();
    selectedNodeItems.clear();
}
