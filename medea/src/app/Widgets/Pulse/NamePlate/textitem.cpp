//
// Created by Cathlyn Aston on 24/9/20.
//

#include "textitem.h"
using namespace Pulse::View;

/**
 * @brief TextItem::TextItem
 * @param text
 * @param parent
 */
TextItem::TextItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent),
      QGraphicsLayoutItem() {}

/**
 * @brief TextItem::setText
 * @param text
 */
void TextItem::setText(const QString& text)
{
    prepareGeometryChange();
    setPlainText(text);
    updateGeometry();
}

/**
 * @brief TextItem::setAlignment
 * This sets this item's text alignment
 * It inherently sets this item's position within its geometry
 * @param alignment
 */
void TextItem::setAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;
    updateGeometry();
}

/**
 * @brief TextItem::setGeometry
 * This gets called whenever the geometry of this item is changed
 * @param geom
 */
void TextItem::setGeometry(const QRectF& geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(getAlignedPos());
}

/**
 * @brief TextItem::sizeHint
 * This sets the values returned by minimum, maximum and preferred sizes
 * @param which
 * @param constraint
 * @return
 */
QSizeF TextItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
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
 * @brief TextItem::getAlignedPos
 * This returns the text bounding rect's position based on the set alignment
 * @return
 */
QPointF TextItem::getAlignedPos() const
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