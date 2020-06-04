#ifndef GRAPHICSLAYOUTITEM_H
#define GRAPHICSLAYOUTITEM_H

#include <QGraphicsLayoutItem>

namespace MEDEA {

class GraphicsLayoutItem : public QGraphicsLayoutItem
{
protected:
    // TODO: Look into the use of a protected only class and if there might be another way we want to write/implement this class
    GraphicsLayoutItem() = default;

    // Required for QGraphicsLayoutItem
    void setGeometry(const QRectF &geom) override = 0;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override = 0;

    int getDefaultWidth() const;
    int getDefaultHeight() const;
    int getPadding() const;

public:
    static const int DEFAULT_GRAPHICS_ITEM_WIDTH = 170;
    static const int DEFAULT_GRAPHICS_ITEM_HEIGHT = 70;
    static const int DEFAULT_GRAPHICS_ITEM_PADDING = 2;
};

}

#endif // GRAPHICSLAYOUTITEM_H