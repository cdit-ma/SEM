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

QRectF NodeView::getVisibleRect( )
{
    QPointF topLeft = mapToScene(0,0);
    QPointF bottomRight = mapToScene(viewport()->width(),viewport()->height());
    return QRectF( topLeft, bottomRight );
}


void NodeView::centreItem(NodeItem *item)
{
    QRectF viewRect = this->getVisibleRect();
    QRectF itemRect = ((QGraphicsItem*)item)->sceneBoundingRect();

    QPointF viewCenter = viewRect.center();
    QPointF itemCenter = itemRect.center();


    //CenterX;
    QScrollBar* xPos=horizontalScrollBar();
    QScrollBar* yPos=verticalScrollBar();

    qreal extraSpace = 1.2;
    qreal scaleRatio = viewRect.height()/ (itemRect.height()*extraSpace);
    scale(scaleRatio, scaleRatio);


    int timeout = 1000;
    bool xRight = false;
    bool yRight = false;

    int xCount = 0;
    int yCount = 0;
    while(xCount++ < timeout){
        viewRect = getVisibleRect();
        viewCenter = viewRect.center();

        if(viewCenter.x() == itemCenter.x()){
            xRight = true;
        }
        else if(viewCenter.x() > itemCenter.x()){
            xPos->setValue(xPos->value() -1);
        }
        else{
            xPos->setValue(xPos->value() +1);
        }
    }
    while(yCount++ < timeout){
        viewRect = getVisibleRect();
        viewCenter = viewRect.center();
        if((viewCenter.y() + 2) > itemCenter.y() && (viewCenter.y()-2) < itemCenter.y()){
            yRight = true;
        }
        else if(viewCenter.y() > itemCenter.y()){
            yPos->setValue(yPos->value() -1);
        }
        else{
            yPos->setValue(yPos->value() +1);
        }
    }
}

void NodeView::mousePressEvent(QMouseEvent *event)
{
    qCritical() << "Mouse Pressed";

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
        emit deletePressed();
    }

    if(event->key() == Qt::Key_Control){
        //Use ScrollHand Drag Mode to enable Panning
        this->CONTROL_DOWN = true;
        emit controlPressed();
    }

    if(event->key() == Qt::Key_Shift){
        this->SHIFT_DOWN = true;
    }


}

void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        emit controlPressed();
    }
    if(event->key() == Qt::Key_Shift){
        this->SHIFT_DOWN = false;
    }
}
