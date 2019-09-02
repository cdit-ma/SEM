#ifndef PIXMAPGRAPHICSITEM_H
#define PIXMAPGRAPHICSITEM_H

#include "graphicslayoutitem.h"
#include <QGraphicsPixmapItem>

class PixmapGraphicsItem : public QGraphicsPixmapItem, public MEDEA::GraphicsLayoutItem
{

public:
    PixmapGraphicsItem(const QPixmap &pixmap, QGraphicsItem* parent = nullptr);

    void updatePixmap(const QPixmap &pixmap);
    void setPixmapPadding(int padding);
    void setSquareSize(int size);

protected:
    // QGraphicsLayoutItem interface
    void setGeometry(const QRectF &geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const  override;

private:
    int pixmap_size_ = getDefaultHeight();
    int pixmap_padding_ = getPadding();
};

#endif // PIXMAPGRAPHICSITEM_H
