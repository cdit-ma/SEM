//
// Created by Cathlyn Aston on 24/9/20.
//

#include "pixmapitem.h"
#include "../pulseviewdefaults.h"

using namespace Pulse::View;
const int padding = 2;

/**
 * @brief PixmapItem::PixmapItem
 * @param pixmap
 * @param parent
 */
PixmapItem::PixmapItem(const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent),
      QGraphicsLayoutItem()
{
    setGraphicsItem(this);
    setTransformationMode(Qt::SmoothTransformation);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

/**
 * @brief PixmapItem::setGeometry
 * @param geom
 */
void PixmapItem::setGeometry(const QRectF& geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());

    // Re-center the pixmap in its geometry
    auto pix_size = pixmap().size();
    auto size_diff =  QSizeF(geom.width() - pix_size.width(), geom.height() - pix_size.height());
    setOffset(size_diff.width() / 2.0, size_diff.height() / 2.0);
}

/**
 * @brief PixmapItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF PixmapItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
            [[fallthrough]];
        case Qt::PreferredSize:
            if (pixmap().isNull()) {
                return Defaults::primary_icon_size;
            } else {
                return pixmap().size() + QSizeF(padding, padding);
            }
        case Qt::MaximumSize:
            return QSizeF(10000, 10000);
        default:
            break;
    }
    return constraint;
}