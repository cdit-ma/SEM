//
// Created by Cathlyn Aston on 24/9/20.
//

#include "pixmapitem.h"
#include "../pulseviewdefaults.h"

using namespace Pulse::View;

/**
 * @brief PixmapItem::PixmapItem
 * @param pixmap
 * @param parent
 */
PixmapItem::PixmapItem(const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent),
      QGraphicsLayoutItem()
{
    setTransformationMode(Qt::SmoothTransformation);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

/**
 * @brief PixmapItem::setGeometry
 * @param geom
 */
void PixmapItem::setGeometry(const QRectF &geom)
{
    QGraphicsLayoutItem::setGeometry(geom);
    if (pixmap().isNull()) {
        setPos(geom.topLeft());
    } else {
        auto half_size = pixmap().size() / 2;
        auto&& offset = geom.center() - QPointF(half_size.width(), half_size.height());
        setPos(offset);
    }
}

/**
 * @brief PixmapItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF PixmapItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
            [[fallthrough]];
        case Qt::PreferredSize:
            if (pixmap().isNull()) {
                return QSizeF(Defaults::minimum_height, Defaults::minimum_height);
            } else {
                return pixmap().size() + QSizeF(Defaults::small_padding, Defaults::small_padding);
            }
        case Qt::MaximumSize:
            return QSizeF(10000, 10000);
        default:
            break;
    }
    return constraint;
}