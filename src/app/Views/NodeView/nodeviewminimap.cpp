#include "nodeviewminimap.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPen>
#include <QTimer>

NodeViewMinimap::NodeViewMinimap(QObject*)
{
    setDragMode(NoDrag);
    setInteractive(false);
    setMouseTracking(true);
    setMinimumSize(130,130);

    auto updateTimer = new QTimer(this);
    updateTimer->setInterval(1000);
    updateTimer->start();
    
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    //connect(updateTimer, &QTimer::timeout, this, &NodeViewMinimap::update);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::NoViewportUpdate);

    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::TextAntialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setRenderHint(QPainter::HighQualityAntialiasing, false);
    setRenderHint(QPainter::NonCosmeticDefaultPen, false);

    setCacheMode(QGraphicsView::CacheBackground);

    zoomPixmap = Theme::theme()->getImage("Icons", "zoom", QSize(16, 16), Qt::white);

    connect(WindowManager::manager(), &WindowManager::activeViewDockWidgetChanged, this, &NodeViewMinimap::activeViewDockWidgetChanged);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NodeViewMinimap::themeChanged);
    themeChanged();
}

void NodeViewMinimap::activeViewDockWidgetChanged(ViewDockWidget *viewDock, ViewDockWidget *prevDock)
{
    //Unattach old view
    if (prevDock) {
        auto prev_node_view = prevDock->getNodeView();
        if (prev_node_view) {
            disconnect(prev_node_view);
            prev_node_view->disconnect(this);
        }
    }

    NodeView* node_view = nullptr;

    //Attach new view
    if (viewDock) {
        node_view = viewDock->getNodeView();
    }

    if (node_view) {
        setBackgroundColor(node_view->getBackgroundColor());
        setScene(node_view->scene());
        
        connect(this, &NodeViewMinimap::minimap_CenterView, node_view, &NodeView::fitToScreen);
        connect(this, &NodeViewMinimap::minimap_Pan, node_view, &NodeView::minimap_Pan);
        connect(this, &NodeViewMinimap::minimap_Zoom, node_view, &NodeView::minimap_Zoom);
        connect(node_view, &NodeView::viewport_changed, this, &NodeViewMinimap::viewportRectChanged);
        connect(node_view, &NodeView::scenerect_changed, this, &NodeViewMinimap::sceneRectChanged);
        
        node_view->update_minimap();
    } else {
        setBackgroundColor(QColor(0,0,0));
        setScene(nullptr);
    }
}

void NodeViewMinimap::centerView()
{
    if (scene()) {
        QRectF rect = scene()->itemsBoundingRect();
        if(rect.isValid()){
            QPointF centerPoint = rect.center();
            qreal width = qMax(1200.0, rect.width());
            qreal height = qMax(800.0, rect.height());

            rect.setWidth(width);
            rect.setHeight(height);
            rect.moveCenter(centerPoint);
            fitInView(rect, Qt::KeepAspectRatio);
            centerOn(centerPoint);
            setSceneRect(rect);
        }
    }
}

void NodeViewMinimap::setMinimapPanning(bool pan)
{
    if(pan != isPanning()){
        setUpdatesEnabled(false);
        _isPanning = pan;

        if(isPanning()){
            setCursor(Qt::ClosedHandCursor);
        }else{
            unsetCursor();
        }
        setUpdatesEnabled(true);
    }
}

void NodeViewMinimap::themeChanged()
{
    setStyleSheet(Theme::theme()->getNodeViewStyleSheet());
}

void NodeViewMinimap::viewportRectChanged(QRectF viewport, qreal zoom)
{
    setUpdatesEnabled(false);
    //Update the viewport Rect.
    viewportRect = viewport;
    zoomPercent = QString::number(zoom * 100, 'f' ,2) + "%";
    setUpdatesEnabled(true);
}

QRectF NodeViewMinimap::zoomIcon() const
{
    QRectF rect;
    rect.setWidth(16);
    rect.setHeight(16);
    rect.moveTopLeft(infoBox().topLeft());
    return rect;
}

QRectF NodeViewMinimap::zoomText() const
{
    return {zoomIcon().topRight(), infoBox().bottomRight()};
}

QRectF NodeViewMinimap::infoBox() const
{
    QRectF rect;
    rect.setWidth(80);
    rect.setHeight(16);
    rect.moveBottomRight(this->rect().bottomRight());
    return rect;
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
    
        //Paint the background
        painter->setBrush(QColor(0, 0, 0, 100));
        painter->setPen(Qt::NoPen);
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
    
    painter->resetTransform();
    
    QFont font;
    font.setBold(true);
    font.setPixelSize(12);
    painter->setFont(font);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawRect(infoBox());
    painter->setPen(Qt::white);
    painter->drawText(zoomText(), Qt::AlignCenter|Qt::AlignLeft, zoomPercent);
    
    QRectF imageRect = zoomIcon();
    painter->drawPixmap(imageRect.x(), imageRect.y(), imageRect.width(), imageRect.height(), zoomPixmap);
}

void NodeViewMinimap::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawRect(rect);
}

void NodeViewMinimap::setEnabled(bool enabled)
{
    drawRect = enabled;
    QGraphicsView::setEnabled(true);
}

void NodeViewMinimap::setBackgroundColor(QColor color)
{
    backgroundColor = std::move(color);
    update();
}

void NodeViewMinimap::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
    
    //Update
    drawRect = scene != nullptr;
    //Center View.
    centerView();
}

bool NodeViewMinimap::isPanning()
{
    return _isPanning;
}

void NodeViewMinimap::sceneRectChanged(QRectF sceneRect)
{
    if(sceneRect.isValid()){
        QPointF centerPoint = sceneRect.center();
        qreal width = qMax(1200.0, sceneRect.width());
        qreal height = qMax(800.0, sceneRect.height());

        sceneRect.setWidth(width);
        sceneRect.setHeight(height);
        sceneRect.moveCenter(centerPoint);
        fitInView(sceneRect, Qt::KeepAspectRatio);
        centerOn(centerPoint);
        setSceneRect(sceneRect);
    }
}

void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton){
        if(viewportContainsPoint(event->pos())){
            previousScenePos = mapToScene(event->pos());
            setMinimapPanning(true);
        }else if(event->button() == Qt::MiddleButton){
            emit minimap_CenterView();
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
    centerView();
}
