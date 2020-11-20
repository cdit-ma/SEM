//
// Created by Cathlyn Aston on 24/9/20.
//

#include "textitem.h"
#include "../pulseviewdefaults.h"

#include <QFontMetrics>

using namespace Pulse::View;
const int padding = 4;

/**
 * @brief TextItem::TextItem
 * @param text
 * @param parent
 */
TextItem::TextItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent),
      QGraphicsLayoutItem()
{
    setGraphicsItem(this);
    setFont(Defaults::primary_font);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

/**
 * @brief TextItem::setGeometry
 * This gets called whenever the geometry of this item is about to change
 * It updates the geometry and adjusts the position so that the text alignment is to the left and vertically center
 * @param geom
 */
void TextItem::setGeometry(const QRectF& geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.left() - padding, geom.top());
}

/**
 * @brief TextItem::sizeHint
 * This sets the values returned by minimum, maximum and preferred sizes
 * @param which
 * @param constraint
 * @return
 */
QSizeF TextItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    // Restrict the height of the item to be the bounding rect of the text plus some padding
    QFontMetrics font_metrics(font());
    qreal text_height = font_metrics.height();

    switch (which) {
        case Qt::MinimumSize:
            [[fallthrough]];
        case Qt::PreferredSize:
            return {boundingRect().width(), text_height + padding * 2};
        case Qt::MaximumSize:
            return {100000, text_height + padding * 2};
        default:
            break;
    }
    return constraint;
}