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


NodeView::NodeView(QWidget *parent):QGraphicsView(parent)
{
    totalScaleFactor = 1;
    NodeType = "";
    rubberBanding = false;
    once = true;
    currentAspect = "";

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
    setSceneRect(0, 0, 5000, 5000);
    translate(2500,2500);
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
    connect(this, SIGNAL(updateViewAspect(QString)), item, SLOT(updateViewAspect(QString)));
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


void NodeView::centreItem(NodeItem *item)
{
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

void NodeView::setViewAspect(QString aspect)
{
    qCritical() << "Setting: " << aspect;
    currentAspect = aspect;
    emit updateViewAspect(aspect);
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
        if(node->isSelected()){
            this->setCursor(Qt::SizeAllCursor);
        }else{
            this->setCursor(Qt::SizeAllCursor);
        }
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


    if(item == 0){
        if(event->button() == Qt::MiddleButton){
            resetTransform();
            int xValue = (horizontalScrollBar()->minimum() + horizontalScrollBar()->maximum())/2;
            int yValue = (verticalScrollBar()->minimum() + verticalScrollBar()->maximum())/2;
            horizontalScrollBar()->setValue(xValue);
            verticalScrollBar()->setValue(yValue);
        }else if(event->button() == Qt::RightButton && CONTROL_DOWN){
            emit unselect();
            emit constructNodeItem(scenePos);
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

}
