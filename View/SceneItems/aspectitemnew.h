#ifndef ASPECTITEMNEW_H
#define ASPECTITEMNEW_H

#include "nodeitemnew.h"

class AspectItemNew : public NodeItemNew
{
    Q_OBJECT
public:
    AspectItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, VIEW_ASPECT aspect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setPos(const QPointF &pos);


    QRectF getElementRect(ELEMENT_RECT rect) const;

    QRectF getResizeRect(RECT_VERTEX vert) const;
    QRectF getExpandStateRect() const;

    int getExpandArrowRotation() const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;

    QRectF getCircleRect() const;

public slots:
    void resetPos();

private:
    QPointF getAspectPos();
    QRectF getMainTextRect() const;
    void setupBrushes();

private:
    QFont mainTextFont;
    QString aspectLabel;
    QColor backgroundColor;
    QColor mainTextColor;
    RECT_VERTEX aspectVertex;

    // EntityItemNew interface
public:
    QRectF currentRect() const;
};

#endif // ASPECTITEMNEW_H
