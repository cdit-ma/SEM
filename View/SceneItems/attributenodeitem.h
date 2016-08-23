#ifndef ATTRIBUTENODEITEM_H
#define ATTRIBUTENODEITEM_H

#include "nodeitemnew.h"



class AttributeNodeItem : public NodeItemNew
{
public:
    AttributeNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);


private:
    qreal height;
    qreal width;
    bool isInstance;

    QFont mainTextFont;

    QRectF mainRect() const;

    QRectF mainIconRect() const;

    QRectF bottomLabelRect() const;

    QRectF labelRect() const;

    QRectF subIconRect() const;


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;

    // EntityItemNew interface
private slots:
    void dataChanged(QString keyName, QVariant data);
};

#endif // ATTRIBUTENODEITEM_H
