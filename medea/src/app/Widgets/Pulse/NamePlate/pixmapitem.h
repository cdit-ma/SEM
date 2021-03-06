//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_PIXMAPITEM_H
#define PULSE_VIEW_PIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>

namespace Pulse::View {

class PixmapItem : public QGraphicsPixmapItem, public QGraphicsLayoutItem {
public:
    explicit PixmapItem(const QPixmap& pixmap = QPixmap(), QGraphicsItem* parent = nullptr);

protected:
    void setGeometry(const QRectF& geom) override;
    [[nodiscard]] QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_PIXMAPITEM_H