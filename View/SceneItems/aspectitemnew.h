#ifndef ASPECTITEMNEW_H
#define ASPECTITEMNEW_H

#include "nodeitemnew.h"
#include <QFont>

class AspectItemNew : public NodeItemNew
{
    Q_OBJECT
public:
    AspectItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, VIEW_ASPECT aspect);

public:
    QRectF getElementRect(ELEMENT_RECT rect);

public slots:
    void resetPos();
private:
    QPointF getAspectPos();

    QRectF getMainTextRect() const;

    QFont mainTextFont;
    QString aspectLabel;
    QColor backgroundColor;
    QColor mainTextColor;
    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // NodeItemNew interface
public:
    void setPos(const QPointF &pos);

    // NodeItemNew interface
public:
    QRectF gridRect() const;
};

#endif // ASPECTITEMNEW_H
