#ifndef ASPECTITEMNEW_H
#define ASPECTITEMNEW_H

#include "nodeitemnew.h"
#include <QFont>

class AspectItemNew : public NodeItemNew
{
    Q_OBJECT
public:
    AspectItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, VIEW_ASPECT aspect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setPos(const QPointF &pos);

    QRectF getElementRect(ELEMENT_RECT rect);
    QRectF getResizeRect(RECT_VERTEX vert) const;

public slots:
    void resetPos();

private:
    QPointF getAspectPos();

    QRectF getMainTextRect() const;

    QFont mainTextFont;
    QString aspectLabel;
    QColor backgroundColor;
    QColor mainTextColor;
    RECT_VERTEX aspectVertex;
};

#endif // ASPECTITEMNEW_H
