#include "nodeviewminimap.h"
#include <QPainter>
#include <QDebug>
#include <QPen>
#define GRID_COUNT 50
#define LINEWIDTH 250
#define GRACE 1000

NodeViewMinimap::NodeViewMinimap(QObject *parent)
{

}

void NodeViewMinimap::updateViewPort(QRectF viewport)
{
    viewport.translate(-GRACE , -GRACE);
    viewport.translate(-LINEWIDTH / 2 , -LINEWIDTH / 2);
    viewport.setWidth(viewport.width() + GRACE + LINEWIDTH);
    viewport.setHeight(viewport.height() + GRACE + LINEWIDTH);

    this->viewport = viewport;
}


void NodeViewMinimap::drawForeground(QPainter *painter, const QRectF &rect)
{
    QPen pen;
    pen.setWidth(LINEWIDTH);
    pen.setColor(QColor(255,0,0));
    painter->setPen(pen);
    painter->drawRect(viewport);

    qreal w_step = viewport.width() / GRID_COUNT;
    qreal h_step = w_step;

    pen.setColor(QColor(0,0,0));
    pen.setWidth(1);
    painter->setPen(pen);
    // draw horizontal grid

    for (qreal y = viewport.top(); y < viewport.bottom(); ) {
        y += w_step;
        painter->drawLine(viewport.left(), y, viewport.right(), y);
    }
    // now draw vertical grid
    for (qreal x = viewport.left(); x < viewport.right(); ) {
        x += h_step;
        painter->drawLine(x, viewport.top(), x, viewport.bottom());
    }
}
