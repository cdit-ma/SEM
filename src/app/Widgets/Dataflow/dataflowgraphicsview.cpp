#include "dataflowgraphicsview.h"
#include "EntityItems/componentinstancegraphicsitem.h"

#include <QtMath>
#include <QDebug>

#define ZOOM_FACTOR 1.05

/**
 * @brief DataflowGraphicsView::DataflowGraphicsView
 * @param parent
 */
DataflowGraphicsView::DataflowGraphicsView(QWidget* parent)
    : QGraphicsView(new QGraphicsScene, parent)
{
    scene()->setParent(this);

    setMinimumSize(600, 400);

    setFocusPolicy(Qt::StrongFocus);
    setCacheMode(QGraphicsView::CacheBackground);

    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


/**
 * @brief DataflowGraphicsView::mousePressEvent
 * @param event
 */
void DataflowGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        panning_ = true;
        prev_pan_origin_ = mapToScene(event->pos());
    }

    QGraphicsView::mousePressEvent(event);
}


/**
 * @brief DataflowGraphicsView::mouseMoveEvent
 * @param event
 */
void DataflowGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (panning_) {
        auto cursor_pos = mapToScene(event->pos());
        auto delta = cursor_pos - prev_pan_origin_;
        translate(delta.x(), delta.y());
        prev_pan_origin_ = mapToScene(event->pos());
    }

    QGraphicsView::mouseMoveEvent(event);
}


/**
 * @brief DataflowGraphicsView::mouseReleaseEvent
 * @param event
 */
void DataflowGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        auto cursor_pos = mapToScene(event->pos());
        auto hit_item = scene()->itemAt(cursor_pos, QTransform());
        if (hit_item != nullptr) {
            if (hit_item->topLevelItem() != nullptr) {
                hit_item = hit_item->topLevelItem();
            }
            centerOn(hit_item);

            // TODO - Need to do additional calculation to center the item within the view
            // if the scene rect is smaller than the viewport rect

            /*
            // NOTE - Moving the center of the scene rect changes its size
            // The scene rect needs to be re-positioned if its size is smaller than the
            // viewport's rect, in order to properly center the viewport on the item
            auto center_point = hit_item->sceneBoundingRect().center();
            auto scene_size = scene()->itemsBoundingRect().size();
            QRectF new_scene_rect(0, 0, scene_size.width(), scene_size.height());
            new_scene_rect.moveCenter(center_point);
            setSceneRect(new_scene_rect);
            */
        }
    }

    panning_ = false;
    prev_pan_origin_ = QPointF(0, 0);

    QGraphicsView::mouseReleaseEvent(event);
}


/**
 * @brief DataflowGraphicsView::wheelEvent
 * @param event
 */
void DataflowGraphicsView::wheelEvent(QWheelEvent *event)
{
    auto delta = event->delta();
    if (delta != 0) {
        qreal scaleFactor = pow(ZOOM_FACTOR, (delta / abs(delta)));
        scale(scaleFactor, scaleFactor);
    }
}
