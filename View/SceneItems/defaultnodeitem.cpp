#include "defaultnodeitem.h"

#include "nodeitemnew.h"
#include <QPainter>
#include <QDebug>

DefaultNodeItem::DefaultNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    qCritical() << "DefaultNodeItem()";
    setMinimumWidth(72);
    setMinimumHeight(72);
    setMarginSize(100);
}

DefaultNodeItem::~DefaultNodeItem()
{

}

void DefaultNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor grey = Qt::gray;

    grey.setAlpha(20);

    painter->setPen(Qt::NoPen);
    painter->setBrush(grey);
    painter->drawRect(boundingRect());

    painter->setBrush(Qt::NoBrush);
    QPen pen;

    pen.setColor(Qt::blue);
    painter->setPen(pen);
    painter->drawRect(expandedRect());

    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawRect(contractedRect());

    pen.setColor(Qt::yellow);
    painter->setPen(pen);
    painter->drawRect(childrenBoundingRect());

    pen.setColor(Qt::green);
    painter->setPen(pen);
    painter->drawEllipse(getCenter(), 10, 10);


}

QRectF DefaultNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    return QRectF();
}

void DefaultNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);
}

