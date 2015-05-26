#include "nodeviewminimap.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QPen>
#define GRID_COUNT 50
//#define LINEWIDTH 400
#define LINEWIDTH 10
#define GRACE 1000

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR
#define MAX_ZOOM_RATIO 50
#define MIN_ZOOM_RATIO 2

NodeViewMinimap::NodeViewMinimap(QObject *parent)
{
    isPanning = false;
    setMouseTracking(true);
}

void NodeViewMinimap::centerView()
{
    if (scene()) {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void NodeViewMinimap::viewportRectChanged(QRectF viewport)
{
    this->viewport = viewport;
}

bool NodeViewMinimap::viewportContainsPoint(QPointF localPos)
{
    QPointF scenePos = mapToScene(localPos.toPoint());
    return viewport.contains(scenePos);
}


void NodeViewMinimap::drawForeground(QPainter *painter, const QRectF &rect)
{
    // this darkens the area in the scene that's not currently visualised by the view
    // it also still draws a rectangle representing what is currently shown in the view
    if (scene()) {
        QRectF scenePath = sceneRect();
        double padding = sceneRect().width()/8;
        scenePath.adjust(-padding, -padding, padding, padding);

        QPainterPath path, viewPath;
        path.addRect(scenePath);
        viewPath.addRect(viewport);
        path -= viewPath;

        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(0,0,0,50)));
        painter->drawPath(path);
        painter->setBrush(Qt::NoBrush);

        QPen pen(QColor(250,250,250));
        pen.setWidth(LINEWIDTH);
        painter->setPen(pen);
        painter->drawRect(viewport);
    }
}


void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{
    if(viewportContainsPoint(event->pos()) && event->button() == Qt::LeftButton){
        isPanning = true;

        lastEventPos = sceneRect().topLeft() - QPointF(10000,10000);


        QPointF newPoint = mapToScene(lastEventPos.toPoint());

        QMouseEvent* fakePress = new QMouseEvent( (QEvent::MouseButtonPress), newPoint.toPoint(),
                Qt::LeftButton,
                Qt::LeftButton,
                Qt::NoModifier);

        minimap_Pressed(fakePress);
        previousScenePosition = event->pos();
    }
    event->setAccepted(true);

}

void NodeViewMinimap::mouseReleaseEvent(QMouseEvent *event)
{
     isPanning = false;


     QMouseEvent* fakePress = new QMouseEvent( (QEvent::MouseButtonRelease), lastEventPos,
             Qt::LeftButton,
             Qt::NoButton,
             Qt::NoModifier);

     minimap_Released(fakePress);
     event->setAccepted(true);
}

void NodeViewMinimap::mouseMoveEvent(QMouseEvent *event)
{

   // if(viewportContainsPoint(event->pos())){
        if(isPanning){

            //event->sc
            QPointF currentPos = event->pos();
            QPointF delta = currentPos - previousScenePosition;
            lastEventPos -= delta;


            QPointF newPoint = mapToScene(lastEventPos.toPoint());

            QMouseEvent *fakePress = new QMouseEvent(QEvent::MouseMove, newPoint.toPoint(),
                    Qt::LeftButton,
                    Qt::LeftButton,
                    Qt::NoModifier);

            minimap_Moved(fakePress);
            //lastEventPos -= delta;
            previousScenePosition = currentPos;
       }
   // }
}

void NodeViewMinimap::wheelEvent(QWheelEvent *event)
{
    minimap_Scrolled(event->delta());
}
