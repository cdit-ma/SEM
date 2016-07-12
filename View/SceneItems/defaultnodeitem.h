#ifndef DEFAULTNODEITEM_H
#define DEFAULTNODEITEM_H

#include "../nodeviewitem.h"
#include "nodeitemnew.h"
#include "entityitemnew.h"




class DefaultNodeItem: public NodeItemNew
{
    Q_OBJECT
public:
    DefaultNodeItem(NodeViewItem* viewItem, NodeItemNew* parentItem);
    ~DefaultNodeItem();

    void setHeaderPadding(QMarginsF bodyPadding);
    QMarginsF getHeaderPadding() const;

    QRectF headerRect() const;
    QRectF bodyRect() const;
    QRectF moveRect() const;
    QRectF expandStateRect() const;

    QRectF getElementRect(ELEMENT_RECT rect);
    QRectF getResizeRect(RECT_VERTEX vert);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void dataChanged(QString keyName, QVariant data);

private:
    void setupBrushes();

    QRectF mainIconRect() const;
    QRectF statusIcon(RECT_VERTEX vert) const;
    QRectF rightLabelRect() const;
    QRectF topLabelRect() const;
    QRectF bottomTextRect() const;

private:
    QMarginsF headerPadding;

};
#endif
