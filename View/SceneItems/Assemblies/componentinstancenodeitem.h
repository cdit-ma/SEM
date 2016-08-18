#ifndef COMPONENTINSTANCENODEITEM_H
#define COMPONENTINSTANCENODEITEM_H

#include "../nodeitemnew.h"



class ComponentInstanceNodeItem : public NodeItemNew
{
public:
    ComponentInstanceNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    QRectF bodyRect() const;
    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;


private:
    qreal height;
    qreal width;
    QFont mainTextFont;

    QRectF mainRect() const;
    QRectF headerRect() const;
    QRectF mainIconRect() const;

private slots:
    void dataChanged(QString keyName, QVariant data);
};

#endif // COMPONENTINSTANCENODEITEM_H
