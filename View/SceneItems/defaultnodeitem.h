#ifndef DEFAULTNODEITEM_H
#define DEFAULTNODEITEM_H

#include "../nodeviewitem.h"
#include "nodeitemnew.h"
#include "entityitemnew.h"


#include <QPainter>
#include <QStyleOptionGraphicsItem>


class DefaultNodeItem: public NodeItemNew
{
    Q_OBJECT
public:
    DefaultNodeItem(NodeViewItem* viewItem, NodeItemNew* parentItem);
    ~DefaultNodeItem();

    QRectF headerRect() const;
    QRectF bodyRect() const;

    QRectF gridRect() const;
    QRectF getElementRect(ELEMENT_RECT rect);
    QRectF getResizeRect(RECT_VERTEX vert);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void dataChanged(QString keyName, QVariant data);
};
#endif
