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

    void setPixmap(const QString& pixmap_name);

    void setPixmapPadding(int padding);
    void setPixmapSize(int width, int height);
    void setAlignment(Qt::Alignment alignment);

protected:
    void setGeometry(const QRectF& geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const override;

private:
    //int pixmap_size_ = getDefaultHeight();
    //int pixmap_padding_ = getPadding();
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_PIXMAPITEM_H
