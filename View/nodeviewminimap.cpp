#include "nodeviewminimap.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QPen>
#define GRID_COUNT 50
#define LINEWIDTH 400
#define GRACE 1000

NodeViewMinimap::NodeViewMinimap(QObject *parent)
{
    isPanning = false;
}

void NodeViewMinimap::viewportRectChanged(QRectF viewport)
{
    //viewport.translate(-GRACE , -GRACE);
    //viewport.translate(-LINEWIDTH / 2 , -LINEWIDTH / 2);
    //viewport.setWidth(viewport.width() + GRACE + LINEWIDTH);
    //viewport.setHeight(viewport.height() + GRACE + LINEWIDTH);
    this->viewport = viewport;
}

bool NodeViewMinimap::viewportContainsPoint(QPointF localPos)
{
    QPointF scenePos = mapToScene(localPos.toPoint());
    return viewport.contains(scenePos);
}


void NodeViewMinimap::drawForeground(QPainter *painter, const QRectF &rect)
{

    QPen pen;
    pen.setWidth(LINEWIDTH);
    pen.setColor(QColor(255,0,0));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(255,255,255,50)));
    painter->drawRect(viewport);


}

void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{


    if(viewportContainsPoint(event->pos())){
        isPanning = true;
        previousScenePosition = mapToScene(event->pos());
    }
    QGraphicsView::mousePressEvent(event);

}

void NodeViewMinimap::mouseReleaseEvent(QMouseEvent *event)
{


     if(viewportContainsPoint(event->pos())){
         isPanning = false;
        qCritical() << "Mouse Releaserd";
    }
     isPanning = false;
     QGraphicsView::mouseReleaseEvent(event);

}

void NodeViewMinimap::mouseMoveEvent(QMouseEvent *event)
{


     if(viewportContainsPoint(event->pos())){
        if(isPanning){
            //qCritical() << "PANNINH";
            QPointF tmp2 = mapToScene(event->pos());
            QPointF tmp = tmp2-mapToScene(previousScenePosition.toPoint());
            viewportRectMoved(-tmp);
            previousScenePosition = event->pos();


        }
    }

     QGraphicsView::mouseMoveEvent(event);

}
