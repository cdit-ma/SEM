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

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    QRectF getElementRect(ELEMENT_RECT rect);

    // EntityItemNew interface
private slots:
    void dataChanged(QString keyName, QVariant data);

    // QGraphicsItem interface
public:

    // NodeItemNew interface

    // QGraphicsItem interface
};
#endif
