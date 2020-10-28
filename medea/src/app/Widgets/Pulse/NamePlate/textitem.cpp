//
// Created by Cathlyn Aston on 24/9/20.
//

#include "textitem.h"
#include "../pulseviewdefaults.h"

#include <QFontMetrics>

using namespace Pulse::View;

/**
 * @brief TextItem::TextItem
 * @param text
 * @param parent
 */
TextItem::TextItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent),
      QGraphicsLayoutItem()
{
    setFont(Defaults::primary_font);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

/**
 * @brief TextItem::setGeometry
 * This gets called whenever the geometry of this item is about to change
 * It updates the item's geometry while keeping the text alignment
 * @param geom
 */
void TextItem::setGeometry(const QRectF& geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
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
    switch (which) {
        case Qt::MinimumSize: {
            QFontMetrics font_metrics(font());
            QSizeF txt_size(font_metrics.horizontalAdvance(toPlainText()), font_metrics.ascent());
            return txt_size; // + QSizeF(Defaults::padding, Defaults::padding);
        }
        case Qt::PreferredSize:
            return boundingRect().size();
        case Qt::MaximumSize:
            return {100000, 10000};
        default:
            break;
    }
    return constraint;
}