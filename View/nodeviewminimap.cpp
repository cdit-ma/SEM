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
#include <QVBoxLayout>
#include <QLabel>

NodeViewMinimap::NodeViewMinimap(QObject*)
{
    isPanning = false;
    setMouseTracking(true);

    //Set QT Options for this QGraphicsView
    //setDragMode(NoDrag);
    //setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    // setContextMenuPolicy(Qt::CustomContextMenu);
    //setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);


    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //setupLayout();
}

void NodeViewMinimap::centerView()
{
    if (scene()) {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}


void NodeViewMinimap::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();

    layout->setSpacing(0);
    layout->setMargin(0);

    QLabel* label = new QLabel("Minimap:");

    label->setStyleSheet("color:#3C3C3C; font-size:10px; font-weight: bold; background-color: rgb(210,210,210);border-bottom: 1px solid;border-color: black;padding-bottom: 1px;");
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
    layout->addStretch();

    setLayout(layout);
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
    QPainterPath path, viewPath;
    path.addRect(rect);
    viewPath.addRect(viewport);
    path -= viewPath;

    QBrush brush(QColor(0,0,0,100));
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);
    painter->drawPath(path);

    QPen pen(Qt::white);
    if (isPanning) {
        pen.setColor(Qt::blue);
    }

    //pen.setWidth(LINEWIDTH);
    pen.setWidth(rect.height()/100);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(viewport);
}


void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(viewportContainsPoint(event->pos())){
            isPanning = true;
            previousScenePos = mapToScene(event->pos());
            emit minimap_Pan();
            update();
        }else{
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void NodeViewMinimap::mouseReleaseEvent(QMouseEvent *event)
{
    isPanning = false;
    emit minimap_Panned();
    update();
}

void NodeViewMinimap::mouseMoveEvent(QMouseEvent *event)
{
    if(isPanning){
        QPoint currentMousePos = event->pos();
        QPointF currentPos = mapToScene(currentMousePos);
        QPointF delta = previousScenePos - currentPos;
        emit minimap_Panning(delta);
        //Update the previous Scene position after the view has panned, such that we get smooth movement.
        previousScenePos = mapToScene(currentMousePos);

    }
}

void NodeViewMinimap::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF previousCenter = viewport.center();

    QPointF currentPos = mapToScene(event->pos());
    QPointF delta = previousCenter - currentPos;
    emit minimap_Pan();
    emit minimap_Panning(delta);
    emit minimap_Panned();
}

void NodeViewMinimap::wheelEvent(QWheelEvent *event)
{
    minimap_Scrolled(event->delta());
}
