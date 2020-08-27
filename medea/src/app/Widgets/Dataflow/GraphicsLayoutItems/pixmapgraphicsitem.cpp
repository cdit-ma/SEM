#include "pixmapgraphicsitem.h"

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
    if (!pixmap.isNull()) {
        auto scaled_pix = pixmap.scaled(pixmap_width_ - pixmap_padding_,
                                        pixmap_height_ - pixmap_padding_,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);
        prepareGeometryChange();
        setPixmap(scaled_pix);
        update();
    }
}

/**
 * @brief PixmapGraphicsItem::setPixmapPadding
 * This sets the padding around this item's pixmap
 * @param padding
 */
void PixmapGraphicsItem::setPixmapPadding(int padding)
{
    auto min_side = qMin(pixmap_width_, pixmap_height_);
    if (padding < min_side) {
        pixmap_padding_ = padding;
        updatePixmap(pixmap());
    } else {
        qWarning("PixmapGraphicsItem::setPixmapPadding - Cannot set the pixmap's padding to bigger than its size");
    }
}

/**
 * @brief PixmapGraphicsItem::setPixmapSize
 * @param width
 * @param height
 */
void PixmapGraphicsItem::setPixmapSize(int width, int height)
{
    pixmap_width_ = width - getPadding();
    pixmap_height_ = height - getPadding();
    updatePixmap(pixmap());
}

/**
 * @brief PixmapGraphicsItem::setPixmapSquareSize
 * This sets this item's preferred size to QSize(size, size)
 * @param size
 */
void PixmapGraphicsItem::setPixmapSquareSize(int size)
{
    setPixmapSize(size, size);
}

/**
 * @brief PixmapGraphicsItem::setGeometry
 * This gets called whenever the geometry of this item is changed
 * It updates the pixmap offset to keep it centralised within this item's geometry
 * @param geom
 */
void PixmapGraphicsItem::setGeometry(const QRectF &geom)
{
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());

    // Work out the offset needed to center the pixmap
    auto diff_width = geom.width() - minimumWidth();
    auto offset_x = 0.0;
    if (diff_width > 0) {
        offset_x = diff_width / 2.0;
    }

    auto diff_height = geom.height() - minimumHeight();
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
            return QSizeF(pixmap_width_, pixmap_height_) - QSizeF(pixmap_padding_, pixmap_padding_);
        case Qt::PreferredSize:
            return QSizeF(pixmap_width_, pixmap_height_);
        case Qt::MaximumSize:
            return QSizeF(10000, 10000);
        default:
            break;
    }
    return constraint;
}