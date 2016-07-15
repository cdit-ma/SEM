#include "nodeviewminimap.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPen>

NodeViewMinimap::NodeViewMinimap(QObject*)
{
    _isPanning = false;
    drawRect = false;

    setDragMode(NoDrag);
    setInteractive(false);
    setMouseTracking(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
}

void NodeViewMinimap::centerView()
{
    if (scene()) {
        QRectF rect = scene()->itemsBoundingRect();
        fitInView(rect, Qt::KeepAspectRatio);
        setSceneRect(rect);
    }
}

void NodeViewMinimap::setMinimapPanning(bool pan)
{
    if(pan != isPanning()){
        setUpdatesEnabled(false);
        _isPanning = pan;
        emit minimap_Panning(pan);

        if(isPanning()){
            setCursor(Qt::ClosedHandCursor);
        }else{
            unsetCursor();
        }
        setUpdatesEnabled(true);
    }
}

void NodeViewMinimap::viewportRectChanged(QRectF viewport)
{
    setUpdatesEnabled(false);
    //Update the viewport Rect.
    viewportRect = viewport;
    setUpdatesEnabled(true);
}


bool NodeViewMinimap::viewportContainsPoint(QPointF localPos)
{
    QPointF scenePos = mapToScene(localPos.toPoint());
    return viewportRect.contains(scenePos);
}


void NodeViewMinimap::drawForeground(QPainter *painter, const QRectF &rect)
{
    if(drawRect){
        QRegion mainArea(rect.toAlignedRect());
        QRegion viewArea(viewportRect.toAlignedRect());
        QRegion maskArea = mainArea.xored(viewArea);

        //Mask off the current viewportRect
        painter->setClipRegion(maskArea);
        painter->setBrush(QColor(0, 0, 0, 150));
        painter->setPen(Qt::NoPen);
        //Paint the background
        painter->drawRect(rect);


        painter->setClipping(false);

        //Paint the viewportRect (With no Brush just a Pen)
        QPen pen(Qt::white);
        pen.setCosmetic(true);
        if(isPanning()) {
            pen.setColor(Qt::blue);
        }
        pen.setJoinStyle(Qt::MiterJoin);
        pen.setWidth(2);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);
        painter->drawRect(viewportRect);
    }
}

void NodeViewMinimap::setEnabled(bool enabled)
{
    drawRect = enabled;
    QGraphicsView::setEnabled(true);
}

void NodeViewMinimap::setScene(QGraphicsScene *scene)
{
    //Center View.
    QGraphicsView::setScene(scene);
    //Update
    drawRect = scene != 0;
}

bool NodeViewMinimap::isPanning()
{
    return _isPanning;
}

void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton){
        if(viewportContainsPoint(event->pos())){
            previousScenePos = mapToScene(event->pos());
            setMinimapPanning(true);
        }
    }
}

void NodeViewMinimap::mouseReleaseEvent(QMouseEvent *)
{
    setMinimapPanning(false);
}

void NodeViewMinimap::mouseMoveEvent(QMouseEvent *event)
{
    if(isPanning()){
        QPointF sceneDelta = previousScenePos - mapToScene(event->pos());

        emit minimap_Pan(sceneDelta);

        //Update the previous Scene position after the view has panned, such that we get smooth movement.
        previousScenePos = mapToScene(event->pos());
    }
}

void NodeViewMinimap::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPointF sceneDelta = viewportRect.center() - mapToScene(event->pos());
        setMinimapPanning(true);
        emit minimap_Pan(sceneDelta);
        setMinimapPanning(false);
    }
}

void NodeViewMinimap::wheelEvent(QWheelEvent *event)
{
    emit minimap_Zoom(event->delta());
}

void NodeViewMinimap::resizeEvent(QResizeEvent *)
{
    //Recenter the Minimap after a widget resize.
    centerView();;
}
