#ifndef PIXMAPGRAPHICSITEM_H
#define PIXMAPGRAPHICSITEM_H

#include "graphicslayoutitem.h"

#include <QGraphicsPixmapItem>

class PixmapGraphicsItem : public QGraphicsPixmapItem, public MEDEA::GraphicsLayoutItem
{
public:
    explicit PixmapGraphicsItem(const QPixmap &pixmap, QGraphicsItem* parent = nullptr);

    void updatePixmap(const QPixmap &pixmap);
    void setPixmapPadding(int padding);
    void setPixmapSize(int width, int height);
    void setPixmapSquareSize(int size);

    void setGeometry(const QRectF &geom) override;

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const override;

private:
    int pixmap_padding_ = getPadding();
    int pixmap_width_ = getDefaultHeight();
    int pixmap_height_ = getDefaultHeight();
};

#endif // PIXMAPGRAPHICSITEM_H