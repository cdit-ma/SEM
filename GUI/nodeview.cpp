#include "nodeview.h"

//Qt includes
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QTouchEvent>
#include <QDebug>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QInputDialog>
#include <QAction>
#include "../Controller/newcontroller.h"

NodeView::NodeView(QWidget *parent):QGraphicsView(parent)
{
    totalScaleFactor = 1;
    NodeType = "";
    rubberBanding = false;
    once = true;

    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    //setDragMode(RubberBandDrag);
    setDragMode(ScrollHandDrag);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    QGraphicsScene* Scene = new QGraphicsScene(this);
    setScene(Scene);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    //Set-up the view
    setSceneRect(0, 0, 19200, 10800);
    translate(9600,5040);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void NodeView::setController(NewController *controller)
{
    this->controller = controller;
}

bool NodeView::getControlPressed()
{
    return this->CONTROL_DOWN;
}

NodeView::~NodeView()
{
    emit view_SetSelectedAttributeModel(0);
}

void NodeView::updateNodeTypeName(QString name)
{
    this->NodeType = name;
    qCritical() << "NodeType: " << name;

    emit this->updateNodeType(NodeType);
}



void NodeView::removeNodeItem(NodeItem *item)
{
    if(scene()->items().contains(item)){
        scene()->removeItem(item);
    }
}



QRectF NodeView::getVisibleRect( )
{
    QPointF topLeft = mapToScene(0,0);
    QPointF bottomRight = mapToScene(viewport()->width(),viewport()->height());
    return QRectF( topLeft, bottomRight );
}


void NodeView::centreItem(GraphMLItem *item)
{
    if(!item){
        qCritical() << "No GUI item to Center";
        return;
    }

    //Get the current Viewport Rectangle
    QRectF viewRect = getVisibleRect();

    //Get the Items Rectangle
    QRectF itemRect = ((QGraphicsItem*)item)->sceneBoundingRect();

    //Extra Space denotes 20% extra space on the height.
    //Calculate the scalre required to fit the item + 20% in the Viewport Rectangle.
    qreal extraSpace = 1.2;
    qreal scaleRatio = viewRect.height()/ (itemRect.height() * extraSpace);
    scale(scaleRatio, scaleRatio);

    //Get the actual Scale Ratio!
    scaleRatio = transform().m22();

    //Get the Center of the ViewPort Rectangle
    QPointF viewCenter = viewRect.center();

    //Get the Center of the Item Rectangle.
    QPointF itemCenter = itemRect.center();

    //Get the updated visible Rectangle.
    viewCenter = getVisibleRect().center();

    //Calculate the distance including scale Ratio between the center of the view and item.
    float deltaX = (itemCenter.x() - viewCenter.x()) * scaleRatio;
    float deltaY = (itemCenter.y() - viewCenter.y()) * scaleRatio;

    //Move the Scroll bars the appropriate distance to square up the items.
    int xBarValue = horizontalScrollBar()->value() + deltaX;
    int yBarValue = verticalScrollBar()->value() + deltaY;

    horizontalScrollBar()->setValue(xBarValue);
    verticalScrollBar()->setValue(yBarValue);
}

void NodeView::clearView()
{
    //scene()->clear();
    //viewport()->update();
}

void NodeView::depthChanged(int depth)
{
    foreach(QGraphicsItem* item, scene()->items()){
        NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);
        if(nodeItem != 0){
            nodeItem->toggleDetailDepth(depth);
        }
    }

}

void NodeView::setRubberBandMode(bool On)
{
    if(On){
        setDragMode(RubberBandDrag);
    }else{
        setDragMode(ScrollHandDrag);
    }


}


void NodeView::setViewAspects(QStringList aspects)
{
    //qCritical() << "Setting: " << aspects;
    currentAspects = aspects;
    emit updateViewAspects(aspects);
}

void NodeView::showContextMenu(QPoint position)
{
    //Got Right Click. Build Menu.
    QPoint globalPos = mapToGlobal(position);
    QPointF scenePos = mapToScene(position);

    menuPosition = scenePos;

    QMenu* rightClickMenu = new QMenu(this);

    QAction* deleteAction = new QAction(this);
    deleteAction->setText("Delete Selection");
    connect(deleteAction, SIGNAL(triggered()), controller, SLOT(view_DeletePressed()));
    rightClickMenu->addAction(deleteAction);

    if(controller->getAdoptableNodeKinds().size() > 0){
        QAction* addChildNode = new QAction(this);
        addChildNode->setText("Create Child Node");
        connect(addChildNode, SIGNAL(triggered()), this, SLOT(view_ConstructNodeAction()));
        rightClickMenu->addAction(addChildNode);
    }

    rightClickMenu->exec(globalPos);
}

void NodeView::view_ConstructNodeGUI(Node *node)
{
    if(!node){
        qCritical() << "Node is Null.";
    }

    Node* parentNode = node->getParentNode();


    NodeItem* parentNodeItem = 0;
    if(parentNode){
        GraphMLItem* parentGUI = getGraphMLItemFromHash(parentNode->getID());
        parentNodeItem = getNodeItemFromGraphMLItem(parentGUI);
    }




   // NodeItem* parentNodeItem = getNodeItemFromNode(parentNode);

    NodeItem* nodeItem = new NodeItem(node, parentNodeItem);

    storeGraphMLItemInHash(nodeItem);
    //nodeItems.append(nodeItem);

    //Connect the Generic Functionality.
    connectGraphMLItemToController(nodeItem, node);

    //Connect the Node Specific Functionality
    connect(nodeItem, SIGNAL(moveSelection(QPointF)), controller, SLOT(view_MoveSelectedNodes(QPointF)));
    connect(controller, SIGNAL(view_SetRubberbandSelectionMode(bool)), nodeItem, SLOT(setRubberbandMode(bool)));
    connect(this, SIGNAL(updateNodeType(QString)), nodeItem, SLOT(updateChildNodeType(QString)));
    connect(this, SIGNAL(updateViewAspects(QStringList)), nodeItem, SLOT(updateViewAspects(QStringList)));


    if(!scene()->items().contains(nodeItem)){
        //Add to model.
        scene()->addItem(nodeItem);
    }
}

void NodeView::view_ConstructEdgeGUI(Edge *edge)
{
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();


    NodeItem* srcGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(src->getID()));
    NodeItem* dstGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(dst->getID()));

    //NodeItem* parentNodeItem = getNodeItemFromGraphMLItem(parentGUI);

//
   // NodeItem* srcGUI = getNodeItemFromNode(src);
   // NodeItem* dstGUI = getNodeItemFromNode(dst);

    if(srcGUI != 0 && dstGUI != 0){
        //We have valid GUI elements for both ends of this edge.

        //Construct a new GUI Element for this edge.
        NodeEdge* nodeEdge = new NodeEdge(edge, srcGUI, dstGUI);

        //Add it to the list of EdgeItems in the Model.
        storeGraphMLItemInHash(nodeEdge);
       //nodeEdges.append(nodeEdge);

        connectGraphMLItemToController(nodeEdge, edge);

        if(nodeEdge){
            nodeEdge->addToScene(scene());
        }
    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }

}

void NodeView::view_DestructGraphMLGUI(QString ID)
{
    removeGraphMLItemFromHash(ID);
}

/*
void NodeView::view_DestructGraphMLGUI(GraphML *graphML)
{
    qCritical() << "DELETING?!";
    if(!graphML){
        qCritical() << "NULL";
    }


    NodeItem* nodeItem = getNodeItemFromGraphML(graphML);
    NodeEdge* nodeEdge = getNodeEdgeFromGraphML(graphML);

    qCritical() << "Got Node Item";
    if(nodeItem){
        int nodeItemPosition = nodeItems.indexOf(nodeItem);
        if(nodeItemPosition != -1){
            nodeItems.removeAt(nodeItemPosition);
        }
    }
    else if(nodeEdge){
        int nodeEdgePosition = nodeEdges.indexOf(nodeEdge);

        if(nodeEdgePosition != -1){
            nodeEdges.removeAt(nodeEdgePosition);
        }
    }else{
        qCritical() << "NOT A VALID GRAPHML";
    }


}
*/

void NodeView::view_SelectGraphML(GraphML *graphML, bool setSelected)
{
    if(graphML){

        GraphMLItem* GUIItem = getGraphMLItemFromHash(graphML->getID());

        if(GUIItem){
            GUIItem->setSelected(setSelected);
            if(setSelected){
                emit view_SetSelectedAttributeModel(GUIItem->getAttributeTable());
                return;
            }
        }

    }
    emit view_SetSelectedAttributeModel(0);

}

void NodeView::view_SortNode(Node *node)
{
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(node->getID()));

    //NodeItem* nodeItem = getNodeItemFromNode(node);
    if(nodeItem){
        nodeItem->sortChildren();
    }

}

void NodeView::view_CenterGraphML(GraphML *graphML)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    centreItem(guiItem);
}

void NodeView::view_SetOpacity(GraphML *graphML, qreal opacity)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        guiItem->setOpacity(opacity);
    }
}

void NodeView::view_ConstructNodeAction()
{

    QStringList nodeKinds = controller->getAdoptableNodeKinds();

    if(nodeKinds.size() > 0){
        bool okay;
        QInputDialog* dialog = new QInputDialog();

        QString nodeKind = dialog->getItem(0, "Selected Node Type","Please Select Child Node Type: ", nodeKinds, 0, false, &okay);

        if(!okay){
            return;
        }
        emit constructNodeItem(nodeKind, menuPosition);
    }else{
        emit controller->view_DialogMessage(MESSAGE_TYPE::WARNING, "No Adoptable Types.");
    }




}

void NodeView::connectGraphMLItemToController(GraphMLItem *GUIItem, GraphML *graphML)
{
    connect(GUIItem, SIGNAL(triggerAction(QString)),  controller, SLOT(view_TriggerAction(QString)));
    connect(GUIItem, SIGNAL(triggerCentered(GraphML*)), this, SLOT(view_CenterGraphML(GraphML*)));
    connect(GUIItem, SIGNAL(triggerSelected(GraphML*, bool)), controller, SLOT(view_GraphMLSelected(GraphML*, bool)));
    connect(GUIItem, SIGNAL(constructGraphMLData(GraphML*,QString)), controller, SLOT(view_ConstructGraphMLData(GraphML*,QString)));
    connect(GUIItem, SIGNAL(destructGraphMLData(GraphML*,QString)), controller, SLOT(view_DestructGraphMLData(GraphML*,QString)));
    connect(GUIItem, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));
    //connect(graphML, SIGNAL(destroyed()), GUIItem, SLOT(destructGraphML()));
}

NodeItem *NodeView::getNodeItemFromGraphMLItem(GraphMLItem *item)
{
    NodeItem* node = dynamic_cast<NodeItem*>(item);
    return node;
}

NodeEdge *NodeView::getEdgeItemFromGraphMLItem(GraphMLItem *item)
{
    NodeEdge* edge = dynamic_cast<NodeEdge*>(item);
    return edge;
}


/*
NodeItem *NodeView::getNodeItemFromGraphML(GraphML *item)
{
    Node* node = getNodeFromGraphML(item);
    return getNodeItemFromNode(node);
}

NodeEdge *NodeView::getNodeEdgeFromGraphML(GraphML *item)
{
    Edge* edge = getEdgeFromGraphML(item);
    return getNodeEdgeFromEdge(edge);
}

Node *NodeView::getNodeFromGraphML(GraphML *item)
{
    Node* node = dynamic_cast<Node*>(item);
    return node;
}

Edge *NodeView::getEdgeFromGraphML(GraphML *item)
{
    Edge* edge = dynamic_cast<Edge*>(item);
    return edge;
}
*/

void NodeView::storeGraphMLItemInHash(GraphMLItem *item)
{
    GraphML* graphML = item->getGraphML();
    if(graphML){
        QString ID = graphML->getID();
        if(guiItems.contains(ID)){
            qCritical() << "Hash already contains GraphMLItem with ID: " << ID;
        }else{
            guiItems[ID] = item;
        }
    }

}

bool NodeView::removeGraphMLItemFromHash(QString ID)
{
    if(guiItems.contains(ID)){
        //qCritical() << "Hash Removed ID: " << ID;
        GraphMLItem* item = guiItems[ID];

        guiItems.remove(ID);

        if(item){
            delete item;
            return true;
        }
    }else{
        qCritical() << "Not HERE?!" << ID;
    }
    return false;
}

GraphMLItem *NodeView::getGraphMLItemFromHash(QString ID)
{
    if(guiItems.contains(ID)){
        return guiItems[ID];
    }else{
        qCritical() << "Cannot find GraphMLItem from Lookup Hash. ID: " << ID;
    }
    return 0;

}


/*

NodeEdge *NodeView::getNodeEdgeFromEdge(Edge *edge)
{
    foreach(NodeEdge* nodeEdge, nodeEdges){
        if(nodeEdge->getGraphML() == edge){
            return nodeEdge;
        }
    }
    return 0;
}

NodeItem *NodeView::getNodeItemFromNode(Node *node)
{
    foreach(NodeItem* nodeItem, nodeItems){
        if(nodeItem->getGraphML() == node){
            return nodeItem;
        }
    }
    return 0;
}
*/

GraphMLItem *NodeView::getGraphMLItemFromGraphML(GraphML *item)
{
    if(item){
        return getGraphMLItemFromHash(item->getID());
    }
    return 0;
}


void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
      QPointF scenePos = this->mapToScene(event->pos());
    if(rubberBanding){
        //QRect selectionRectangle(origin, scenePos.toPoint());
        //QPainterPath pp;
        //pp.addRect(selectionRectangle);


        //scene()->setSelectionArea(pp,Qt::ContainsItemBoundingRect);


    }
    QGraphicsView::mouseReleaseEvent(event);
}

void NodeView::mouseMoveEvent(QMouseEvent *event)
{

    QPointF scenePos = this->mapToScene(event->pos());

    QGraphicsItem* item = this->scene()->itemAt(scenePos,this->transform());

    NodeItem* node = dynamic_cast<NodeItem*>(item);

    if(node){
        this->setCursor(Qt::SizeAllCursor);
    }else{
        this->setCursor(Qt::ArrowCursor);
    }

    QGraphicsView::mouseMoveEvent(event);
}

void NodeView::mousePressEvent(QMouseEvent *event)
{

    QPointF scenePos = this->mapToScene(event->pos());
    QGraphicsItem* item = this->scene()->itemAt(scenePos,this->transform());
    rubberBanding = false;


    if(!item){
        if(event->button() == Qt::MiddleButton){
            if(CONTROL_DOWN){
                emit sortModel();
            }else{
                resetTransform();
                int xValue = (horizontalScrollBar()->minimum() + horizontalScrollBar()->maximum())/2;
                int yValue = (verticalScrollBar()->minimum() + verticalScrollBar()->maximum())/2;
                horizontalScrollBar()->setValue(xValue);
                verticalScrollBar()->setValue(yValue);
            }
        }else if(event->button() == Qt::RightButton && CONTROL_DOWN){
            //emit unselect();
            //emit constructNodeItem(scenePos);
        }else if( event->button() == Qt::LeftButton && CONTROL_DOWN){
            origin = scenePos.toPoint();
        }else{
            emit unselect();
        }
    }


    QGraphicsView::mousePressEvent(event);
}

void NodeView::wheelEvent(QWheelEvent *event)
{
    if(CONTROL_DOWN){
        // Scale the view / do the zoom
        double scaleFactor = 1.05;
        if(event->delta() > 0) {
            // Zoom in
            scale(scaleFactor, scaleFactor);
        } else {
            // Zooming out
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        emit updateZoom(transform().m22());
    }else{
        //QGraphicsView::wheelEvent(event);
    }
}

void NodeView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete){
        emit deletePressed(true);
    }

    if(event->key() == Qt::Key_Control){
        //Use ScrollHand Drag Mode to enable Panning
        this->CONTROL_DOWN = true;
        emit controlPressed(true);
    }

    if(event->key() == Qt::Key_Escape){
        emit escapePressed(true);
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_A){
        emit selectAll();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_C){
       // emit copy();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_X){
        //emit cut();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_V){
        //emit paste();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_Z){
        //emit undo();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_Y){
        //emit redo();
    }

    if(event->key() == Qt::Key_Shift){
        this->SHIFT_DOWN = true;
        emit shiftPressed(true);
    }


}

void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        this->CONTROL_DOWN = false;
        emit controlPressed(false);
    }
    if(event->key() == Qt::Key_Shift){
        this->SHIFT_DOWN = false;
        emit shiftPressed(false);
    }
    /*
    if(event->key() == Qt::Key_Escape){
        emit escapePressed(false);
    }
    */


}

void NodeView::view_Refresh()
{
    scene()->update();
}

/*
bool NodeView::guiCreated(GraphML *item)
{
    return getNodeItemFromGraphML(item) || getNodeEdgeFromGraphML(item);
}
*/

void NodeView::view_ConstructGraphMLGUI(GraphML *item)
{
    Node* node = dynamic_cast<Node*>(item);
    Edge* edge = dynamic_cast<Edge*>(item);
    if(node){
        view_ConstructNodeGUI(node);
    }else if(edge){
        view_ConstructEdgeGUI(edge);
    }else{
        qCritical() << "Unknown Type";
    }

}


void NodeView::printErrorText(GraphML *graphml, QString text)
{
    //TODO
    Q_UNUSED(graphml);
    Q_UNUSED(text);
}
