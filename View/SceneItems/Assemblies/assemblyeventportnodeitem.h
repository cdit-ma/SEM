#ifndef ASSEMBLYEVENTPORTNODEITEM_H
#define ASSEMBLYEVENTPORTNODEITEM_H

#include "../containerelementnodeitem.h"

class AssemblyEventPortNodeItem : public ContainerElementNodeItem
{
public:
    AssemblyEventPortNodeItem(NodeViewItem* viewItem, NodeItemNew* parentItem);

    void setRightJustified(bool isRight);

    QRectF getElementRect(ELEMENT_RECT rect) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QRectF iconRect() const;
    QRectF textRect() const;

    QRectF subIconRect() const;
    QRectF topSubIconRect() const;
    QRectF midSubIconRect() const;
    QRectF bottomSubIconRect() const;

    QFont mainTextFont;

    // NodeItemNew interface
public:
};

#endif // ASSEMBLYEVENTPORTNODEITEM_H
