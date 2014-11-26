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
#include "nodeconnection.h"
#include <QMenu>
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
    qCritical() << "Killed Node View";
}

void NodeView::updateNodeTypeName(QString name)
{
    this->NodeType = name;
    qCritical() << "NodeType: " << name;

    emit this->updateNodeType(NodeType);
}


void NodeView::addNodeItem(NodeItem *item)
{
    if(!scene()->items().contains(item)){
        scene()->addItem(item);
        //Add to model.
    }
    connect(this, SIGNAL(updateNodeType(QString)), item, SLOT(updateChildNodeType(QString)));
    connect(this, SIGNAL(updateViewAspects(QStringList)), item, SLOT(updateViewAspects(QStringList)));
}

void NodeView::removeNodeItem(NodeItem *item)
{
    if(scene()->items().contains(item)){
        scene()->removeItem(item);
    }
}


void NodeView::addEdgeItem(NodeEdge* edge){
    //qCritical() << "View: Adding NodeConnection to View";
    if(edge != 0){
        edge->addToScene(scene());
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

void NodeView::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    QPointF scenePos = this->mapToScene(pos);

    QMenu myMenu;

    myMenu.addAction("Delete Selection");
    myMenu.addAction("Add Child Node");

    // ...

    QAction* selectedItem = myMenu.exec(globalPos);
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
    //myMenu.exec();
}

void NodeView::view_ConstructNodeGUI(Node *node)
{
    if(!node){
        qCritical() << "Node is Null.";
    }

    Node* parentNode = node->getParentNode();

    NodeItem* parentNodeItem = getNodeItemFromNode(parentNode);

    NodeItem* nodeItem = new NodeItem(node, parentNodeItem);

    nodeItems.append(nodeItem);

    connect(nodeItem, SIGNAL(setItemSelected(GraphML*, bool)), controller, SLOT(view_SetItemSelected(GraphML*,bool)));
    connect(nodeItem, SIGNAL(actionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));

    connect(nodeItem, SIGNAL(center(GraphML*)), this, SLOT(view_SetCentered(GraphML*)));

    connect(nodeItem, SIGNAL(makeChildNode(QPointF)), controller, SLOT(view_ConstructChildNode(QPointF)));
    connect(nodeItem, SIGNAL(moveSelection(QPointF)), controller, SLOT(view_MoveSelectedNodes(QPointF)));

    connect(node, SIGNAL(destroyed()), nodeItem, SLOT(destructNodeItem()));
    connect(controller, SIGNAL(view_SetRubberbandSelectionMode(bool)), nodeItem, SLOT(setRubberbandMode(bool)));


    connect(nodeItem, SIGNAL(destructGraphMLData(GraphML*,QString)), controller, SLOT(view_DestructGraphMLData(GraphML*,QString)));
    connect(nodeItem, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));


    addNodeItem(nodeItem);
}

void NodeView::view_ConstructEdgeGUI(Edge *edge)
{
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    NodeItem* srcGUI = getNodeItemFromNode(src);
    NodeItem* dstGUI = getNodeItemFromNode(dst);

    if(srcGUI != 0 && dstGUI != 0){
        //We have valid GUI elements for both ends of this edge.

        //Construct a new GUI Element for this edge.
        NodeEdge* nodeEdge = new NodeEdge(edge, srcGUI, dstGUI);

        //Add it to the list of EdgeItems in the Model.
        nodeEdges.append(nodeEdge);

        connect(edge, SIGNAL(destroyed()), nodeEdge, SLOT(destructNodeEdge()));
        connect(nodeEdge, SIGNAL(center(GraphML*)), this, SLOT(view_SetCentered(GraphML*)));

        connect(nodeEdge, SIGNAL(actionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));
        connect(nodeEdge, SIGNAL(setItemSelected(GraphML*,bool)), controller, SLOT(view_SetItemSelected(GraphML*,bool)));

        connect(nodeEdge, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));

       // connect(nodeEdge, SIGNAL(setSelected(Edge*,bool)), this, SLOT(view_SetEdgeSelected(Edge*,bool)));

        addEdgeItem(nodeEdge);
    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }

}

void NodeView::view_DestructGraphMLGUI(GraphML *graphML)
{
    NodeItem* nodeItem = getNodeItemFromGraphML(graphML);
    NodeEdge* nodeEdge = getNodeEdgeFromGraphML(graphML);

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

void NodeView::view_SetSelected(GraphML *graphML, bool setSelected)
{
    GraphMLItem* GUIItem = getGraphMLItemFromGraphML(graphML);

    if(GUIItem){
        GUIItem->setSelected(setSelected);
    }

    if(setSelected){
        emit view_SetSelectedAttributeModel(GUIItem->getAttributeTable());
    }else{
        emit view_SetSelectedAttributeModel(0);
    }
}

void NodeView::view_SortNode(Node *node)
{
    NodeItem* nodeItem = getNodeItemFromNode(node);
    if(nodeItem){
        nodeItem->sortChildren();
    }

}

void NodeView::view_SetCentered(GraphML *graphML)
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

GraphMLItem *NodeView::getGraphMLItemFromGraphML(GraphML *item)
{
    NodeItem* nodeItem = getNodeItemFromGraphML(item);
    NodeEdge* nodeEdge = getNodeEdgeFromGraphML(item);
    if(nodeItem){
        return nodeItem;
    }else if(nodeEdge){
        return nodeEdge;
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

bool NodeView::guiCreated(GraphML *item)
{
    return getNodeItemFromGraphML(item) || getNodeEdgeFromGraphML(item);
}

void NodeView::printErrorText(GraphML *graphml, QString text)
{
    /*
    if(node){
        QGraphicsTextItem * io = new QGraphicsTextItem();
        io->setPos(node->scenePos());
        io->setPlainText(text);
        io->setTextWidth(20);

        scene()->addItem(io);
    }
    */
}
