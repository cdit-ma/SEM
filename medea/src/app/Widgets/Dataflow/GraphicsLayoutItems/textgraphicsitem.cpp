#include "textgraphicsitem.h"
#include <QFont>

/**
 * @brief TextGraphicsItem::TextGraphicsItem
 * @param text
 * @param parent
 */
TextGraphicsItem::TextGraphicsItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent),
      MEDEA::GraphicsLayoutItem()
{
    setFont(QFont("Verdana", 12));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

/**
 * @brief TextGraphicsItem::setAlignment
 * This sets this item's text alignment
 * It inherently sets this item's position within its geometry
 * @param alignment
 */
void TextGraphicsItem::setTextAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;
    updateGeometry();
}

/**
 * @brief TextGraphicsItem::setGeometry
 * This gets called whenever the geometry of this item is changed
 * @param geom
 */
void TextGraphicsItem::setGeometry(const QRectF& geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(getAlignedPos());
}

/**
 * @brief TextGraphicsItem::sizeHint
 * This sets the values returned by minimum, maximum and preferred sizes
 * @param which
 * @param constraint
 * @return
 */
QSizeF TextGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return boundingRect().size();
        case Qt::MaximumSize:
            return {100000, 10000};
        default:
            break;
    }
    return constraint;
}

/**
 * @brief TextGraphicsItem::getAlignedPos
 * This returns the text bounding rect's position based on the set alignment
 * @return
 */
QPointF TextGraphicsItem::getAlignedPos() const
{
    const auto& geom = geometry();
    const auto w = boundingRect().width();
    const auto h = boundingRect().height();

    QPointF aligned_pos(geom.left(), geom.top());
    if (alignment_ & Qt::AlignRight) {
        aligned_pos.setX(geom.right() - w);
    }
    if (alignment_ & Qt::AlignBottom) {
        aligned_pos.setY(geom.bottom() - h);
    }

    // NOTE: Qt::AlignCenter = Qt::AlignHCenter | Qt::AlignVCenter
    if (alignment_ & Qt::AlignHCenter) {
        aligned_pos.setX(geom.center().x() - w / 2.0);
    }
    if (alignment_ & Qt::AlignVCenter) {
        aligned_pos.setY(geom.center().y() - h / 2.0);
    }

    return aligned_pos;
}