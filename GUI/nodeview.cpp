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



NodeView::NodeView(QWidget *parent):QGraphicsView(parent)
{
    totalScaleFactor = 1;

    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    //setDragMode(RubberBandDrag);
    setDragMode(ScrollHandDrag);
    //setDragMode()

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    QGraphicsScene* Scene = new QGraphicsScene(this);
    setScene(Scene);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    //Set-up the view
    setSceneRect(0, 0, 5000, 5000);
    translate(2500,2500);

}

void NodeView::addNodeItem(NodeItem *item)
{
    qCritical() << "View: Adding NodeItem to View";
    if(!scene()->items().contains(item)){
        scene()->addItem(item);
    }
    qCritical() << "CRASGH: Adding NodeItem to View";
}

void NodeView::removeNodeItem(NodeItem *item)
{
    if(scene()->items().contains(item)){
        scene()->removeItem(item);
    }
}


void NodeView::addEdgeItem(NodeEdge* edge){
    qCritical() << "View: Adding NodeConnection to View";
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

void NodeView::mousePressEvent(QMouseEvent *event)
{

    QPointF scenePos = this->mapToScene(event->pos());
    QGraphicsItem* item = this->scene()->itemAt(scenePos,this->transform());

    if(item == 0){
        if(event->button() == Qt::MiddleButton){
            resetTransform();
            int xValue = (horizontalScrollBar()->minimum() + horizontalScrollBar()->maximum())/2;
            int yValue = (verticalScrollBar()->minimum() + verticalScrollBar()->maximum())/2;
            horizontalScrollBar()->setValue(xValue);
            verticalScrollBar()->setValue(yValue);
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
        emit copy();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_X){
        emit cut();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_V){
        emit paste();
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
