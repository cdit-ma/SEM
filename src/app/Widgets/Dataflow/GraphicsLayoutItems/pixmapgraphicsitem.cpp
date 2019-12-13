#include "pixmapgraphicsitem.h"

#include <QDebug>

/**
 * @brief PixmapGraphicsItem::PixmapGraphicsItem
 * @param pixmap
 * @param parent
 */
PixmapGraphicsItem::PixmapGraphicsItem(const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent),
      MEDEA::GraphicsLayoutItem()
{
    updatePixmap(pixmap);
    setTransformationMode(Qt::SmoothTransformation);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}


/**
 * @brief PixmapGraphicsItem::updatePixmap
 * This scales and sets this item's Pixmap
 * @param pixmap
 */
void PixmapGraphicsItem::updatePixmap(const QPixmap& pixmap)
{
    auto scaled_pix = pixmap.scaledToHeight(pixmap_size_ - pixmap_padding_,
                                    Qt::TransformationMode::SmoothTransformation);
    prepareGeometryChange();
    setPixmap(scaled_pix);
    update();
}


/**
 * @brief PixmapGraphicsItem::setPixmapPadding
 * This sets the padding around this item's pixmap
 * @param padding
 */
void PixmapGraphicsItem::setPixmapPadding(int padding)
{
    if (padding < pixmap_size_) {
        pixmap_padding_ = padding;
        updatePixmap(pixmap());
    } else {
        qWarning("PixmapGraphicsItem::setPixmapPadding - Cannot set the pixmap's padding to bigger than its size");
    }
}


/**
 * @brief PixmapGraphicsItem::setSquareSize
 * This sets this item's preferred size to QSize(size, size)
 * @param size
 */
void PixmapGraphicsItem::setSquareSize(int size)
{
    pixmap_size_ = size - getPadding();
    updatePixmap(pixmap());
}


/**
 * @brief PixmapGraphicsItem::setGeometry
 * This gets called whenever the geometry of this item is changed
 * It updates the pixmap offset to keep it centralised within this item's geometry
 * @param geom
 */
void PixmapGraphicsItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());

    // Work out the offset needed to center the pixmap
    auto diff_width = geom.width() - boundingRect().width();
    auto offset_x = 0.0;
    if (diff_width > 0) {
        offset_x = diff_width / 2.0;
    }

    auto diff_height = geom.height() - boundingRect().height();
    auto offset_y = 0.0;
    if (diff_height > 0) {
        offset_y = diff_height / 2.0;
    }

    setOffset(offset_x, offset_y);
}


/**
 * @brief PixmapGraphicsItem::sizeHint
 * This sets the values returned by minimum, maximum and preferred sizes
 * @param which
 * @param constraint
 * @return
 */
QSizeF PixmapGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        return QSizeF(pixmap_size_, pixmap_size_);
    case Qt::MaximumSize:
        return QSizeF(10000, 10000);
    default:
        break;
    }
    return constraint;
}
