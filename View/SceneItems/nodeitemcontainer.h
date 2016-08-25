#ifndef COMPONENTINSTANCENODEITEM_H
#define COMPONENTINSTANCENODEITEM_H

#include "nodeitemnew.h"



class NodeItemContainer : public NodeItemNew
{
public:
    NodeItemContainer(NodeViewItem *viewItem, NodeItemNew *parentItem);

    QRectF bodyRect() const;
    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;

    void setRightIcon(bool isRight);


private:
    qreal height;
    qreal width;
    QFont mainTextFont;
    bool expanded;
    bool rightIcon;

    QRectF mainRect() const;
    QRectF headerRect() const;

    QRectF headerTextRect() const;
    QRectF outerIconRect() const;
    QRectF innerIconRect() const;

    QRectF innerLabelRect() const;
    QRectF outerLabelRect() const;
    QRectF innerKindRect() const;
    QRectF outerKindRect() const;
    QRectF deployedRect() const;
    QRectF qosRect() const;
    QRectF expandStateRect() const;

private slots:
    void dataChanged(QString keyName, QVariant data);

    // EntityItemNew interface
public:
    QPointF getCenterOffset() const;
};

#endif // COMPONENTINSTANCENODEITEM_H
